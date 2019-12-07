#include "PlayerController.hpp"

#include "../events/ClientDisconnectedEvent.hpp"
#include "../events/EventController.hpp"
#include "../factories/IdFactory.hpp"
#include "../util/Util.hpp"
#include "../util/game_settings.hpp"
#include <exception>

PlayerController::PlayerController(std::vector<std::shared_ptr<Player>>& players) :
    m_players(players) {
    addEventListeners();
}

PlayerController::~PlayerController() {
}

void PlayerController::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_CLIENT_DISCONNECTED,
        std::bind(&PlayerController::handleClientDisconnectedEvent, this, std::placeholders::_1));
}

uint32_t PlayerController::createPlayer(uint32_t client_id) {
    size_t next_player_index = nextPlayerIndex++;
    if (next_player_index >= m_players.size()) {
        throw std::out_of_range("Exceeded max number of players.");
    }

    uint32_t new_player_id = m_players[next_player_index]->getId();
    getPlayer(new_player_id)->setActive(true);
    return new_player_id;
}

void PlayerController::handleClientDisconnectedEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<ClientDisconnectedEvent> client_disconnect_event =
        std::dynamic_pointer_cast<ClientDisconnectedEvent>(event);
    uint32_t player_id = client_disconnect_event->getPlayerId();
    removePlayer(player_id);
}

void PlayerController::removePlayer(uint32_t player_id) {
    getPlayer(player_id)->setActive(false);
}

void PlayerController::update(const InputDef::PlayerInputs& pi) {
    for (const auto& player_unreliable_input : pi.player_unreliable_inputs) {
        uint32_t player_id = player_unreliable_input.player_id;
        auto ui = player_unreliable_input.unreliable_input;
        sf::Vector2f direction = Util::inputToDirection(
            ui.toggle_up, ui.toggle_down, ui.toggle_right, ui.toggle_left, ui.toggle_stop);
        getPlayer(player_id)->setDirection(direction);
    }
    for (const auto& player_reliable_input : pi.player_reliable_inputs) {
        uint32_t player_id = player_reliable_input.player_id;
        auto player = getPlayer(player_id);
        player->setJoinable(player->getJoinable() ^
                            player_reliable_input.reliable_input.toggle_joinable);
        player->setUngroup(player->getUngroup() ^
                           player_reliable_input.reliable_input.toggle_ungroup);
    }
}

void PlayerController::updatePostPhysics() {
    // noop
}

std::shared_ptr<Player> PlayerController::getPlayer(uint32_t player_id) {
    return m_players[IdFactory::getInstance().getIndex(player_id)];
}

std::vector<uint32_t> PlayerController::getActivePlayerIds() const {
    std::vector<uint32_t> active_player_ids;
    for (auto&& player : m_players) {
        if (player->isActive()) {
            active_player_ids.push_back(player->getId());
        }
    }
    return active_player_ids;
}