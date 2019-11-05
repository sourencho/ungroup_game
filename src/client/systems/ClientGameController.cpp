#include <thread>

#include "../../common/events/CollisionEvent.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/physics/VectorUtil.hpp"
#include "../../common/util/game_state.hpp"
#include "ClientGameController.hpp"
#include <SFML/Graphics.hpp>

ClientGameController::ClientGameController(size_t max_player_count, size_t max_mine_count,
                                           Keys keys)
    : GameController(max_player_count, max_mine_count), m_NetworkingClient(new NetworkingClient()),
      m_AnimationController(new AnimationController()), m_Keys(keys) {
    addEventListeners();
}

ClientGameController::~ClientGameController() {}

void ClientGameController::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_COLLISION,
        std::bind(&ClientGameController::handleCollisionEvent, this, std::placeholders::_1));
}

ClientInputs ClientGameController::collectInputs() {
    return getClientInputs(m_ClientReliableUpdate, m_ClientUnreliableUpdate);
}

void ClientGameController::setNetworkState() {
    // noop
}

void ClientGameController::incrementTick() { m_NetworkingClient->incrementTick(); }

void ClientGameController::updateView(sf::RenderWindow& window,
                                      sf::Vector2f buffer_scaling_factor) {
    // Update view to match player's group's position
    sf::Vector2f group_position =
        m_GameObjectStore->getGroup(m_GroupController->getGroupId(m_PlayerId))->getCenter();
    sf::View view = window.getView();
    sf::Vector2f group_view_coordinates = {group_position.x * buffer_scaling_factor.x,
                                           group_position.y * buffer_scaling_factor.y};
    view.setCenter(group_view_coordinates);
    window.setView(view);
}

void ClientGameController::draw(sf::RenderTarget& target) {
    m_GroupController->draw(target);
    m_MineController->draw(target);
    m_AnimationController->draw(target);
}

void ClientGameController::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (sf::Keyboard::isKeyPressed(m_Keys.group)) {
            m_ClientReliableUpdate.joinable ^= true;
        }

        sf::Vector2f direction = sf::Vector2f(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(m_Keys.up)) {
            direction += sf::Vector2f(0.f, -1.f);
        }
        if (sf::Keyboard::isKeyPressed(m_Keys.down)) {
            direction += sf::Vector2f(0.f, 1.f);
        }
        if (sf::Keyboard::isKeyPressed(m_Keys.left)) {
            direction += sf::Vector2f(-1.f, 0.f);
        }
        if (sf::Keyboard::isKeyPressed(m_Keys.right)) {
            direction += sf::Vector2f(1.f, 0.f);
        }
        direction = VectorUtil::normalize(direction);
        m_ClientUnreliableUpdate.direction = direction;
    }
}

/**
 * Fetch player id from server and set the client updates to send to the server then update game
 * state. This is either done via rewind and replay when we have a game state update from the server
 * or via local interpolation.
 * Rewind and replay is applying the game state update from the server (rewind because it is likely
 * for an old tick) and then interpolating up to the current tick via interpolation (replay).
 **/
void ClientGameController::update() {
    fetchPlayerId();
    setClientUpdates();

    if (m_NetworkingClient->getGameStateIsFresh()) {
        rewindAndReplay();
    }
    GameController::update();
}

void ClientGameController::step(const ClientInputs& cis, sf::Int32 delta_ms) {
    computeGameState(cis, delta_ms);
    m_AnimationController->step(delta_ms);
}

void ClientGameController::rewindAndReplay() {
    // Rewind
    GameState game_state = m_NetworkingClient->getGameState();
    applyGameState(game_state);

    // Replay
    int tick_delta = getTick() - game_state.tick;
    if (tick_delta <= 0) {
        return;
    } // If the client is behind the server we don't need to replay

    // Loop through ticks that need to be replayed and apply client input from cache if present
    for (int i = 0; i < tick_delta; ++i) {
        ClientInputAndTick client_input_and_tick;
        unsigned int replay_tick = game_state.tick + i;

        if (m_TickToInput.count(replay_tick) > 0) {
            client_input_and_tick = m_TickToInput[replay_tick];
            GameController::computeGameState(
                getClientInputs(client_input_and_tick.cru, client_input_and_tick.cuu),
                GameController::MIN_TIME_STEP);
        } else {
            // If we don't have input for this tick pass in empty ClientInputs
            GameController::computeGameState(ClientInputs(), GameController::MIN_TIME_STEP);
        }
    }
    m_TickToInput.clear();
}

void ClientGameController::fetchPlayerId() {
    if (m_PlayerId == -1) {
        m_PlayerId = m_NetworkingClient->getPlayerId();
        if (m_PlayerId != -1) {
            m_PlayerController->setPlayerClient(m_PlayerId, m_NetworkingClient->getClientId());
        }
    }
}

void ClientGameController::setClientUpdates() {
    // Set input to send to server
    m_NetworkingClient->setClientUnreliableUpdate(m_ClientUnreliableUpdate);
    m_NetworkingClient->setClientReliableUpdate(m_ClientReliableUpdate);

    // Save input and state for replay
    m_TickToInput[m_NetworkingClient->getTick()] = (ClientInputAndTick){
        m_ClientUnreliableUpdate, m_ClientReliableUpdate, m_NetworkingClient->getTick()};
}

ClientInputs& ClientGameController::getClientInputs(ClientReliableUpdate cru,
                                                    ClientUnreliableUpdate cuu) {
    int client_id = static_cast<int>(m_NetworkingClient->getClientId());

    if (m_ClientInputs.client_id_and_reliable_updates.size() == 0) {
        m_ClientInputs.client_id_and_reliable_updates.resize(1);
    }
    m_ClientInputs.client_id_and_reliable_updates[0].client_id = client_id;
    m_ClientInputs.client_id_and_reliable_updates[0].client_reliable_update = cru;

    if (m_ClientInputs.client_id_and_unreliable_updates.size() == 0) {
        m_ClientInputs.client_id_and_unreliable_updates.resize(1);
    }
    m_ClientInputs.client_id_and_unreliable_updates[0].client_id = client_id;
    m_ClientInputs.client_id_and_unreliable_updates[0].client_unreliable_update = cuu;

    return m_ClientInputs;
}

void ClientGameController::handleCollisionEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<CollisionEvent> collision_event =
        std::dynamic_pointer_cast<CollisionEvent>(event);

    createCollisionAnimation(collision_event->getCollision().position);
}

void ClientGameController::createCollisionAnimation(const sf::Vector2f& collision_position) {
    auto collision_sprite = std::unique_ptr<AnimatedSprite>(new AnimatedSprite(
        *m_ResourceStore->getTexture("collision"), {6, 1}, 240, collision_position, {2.f, 2.f}));
    m_AnimationController->add(std::move(collision_sprite));
}

unsigned int ClientGameController::getTick() { return m_NetworkingClient->getTick(); }

void ClientGameController::setTick(unsigned int tick) { m_NetworkingClient->setTick(tick); }
