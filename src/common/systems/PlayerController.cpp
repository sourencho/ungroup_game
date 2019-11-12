#include "PlayerController.hpp"

#include "../events/ClientDisconnectedEvent.hpp"
#include "../events/EventController.hpp"
#include "../factories/IdFactory.hpp"
#include "../util/Util.hpp"
#include "../util/game_settings.hpp"
#include <exception>

PlayerController::PlayerController(std::vector<std::shared_ptr<Player>>& players)
    : m_players(players) {
    addEventListeners();
}

PlayerController::~PlayerController() {}

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

void PlayerController::removePlayer(uint32_t player_id) { getPlayer(player_id)->setActive(false); }

void PlayerController::update(std::shared_ptr<PlayerInputs> pi) {
    for (const auto& player_unreliable_update : pi->player_unreliable_updates) {
        uint32_t player_id = player_unreliable_update.player_id;
        auto cuu = player_unreliable_update.client_unreliable_update;
        sf::Vector2f direction = Util::inputToDirection(
            cuu.toggle_up, cuu.toggle_down, cuu.toggle_right, cuu.toggle_left, cuu.toggle_stop);
        getPlayer(player_id)->setDirection(direction);
    }
    for (const auto& player_reliable_update : pi->player_reliable_updates) {
        uint32_t player_id = player_reliable_update.player_id;
        auto player = getPlayer(player_id);
        player->setJoinable(player->getJoinable() ^
                            player_reliable_update.client_reliable_update.toggle_joinable);
        player->setUngroup(player->getUngroup() ^
                           player_reliable_update.client_reliable_update.toggle_ungroup);
    }
}

void PlayerController::updatePostPhysics() {
    // noop
}

std::shared_ptr<Player> PlayerController::getPlayer(uint32_t player_id) {
    return m_players[IdFactory::getInstance().getIndex(player_id)];
}