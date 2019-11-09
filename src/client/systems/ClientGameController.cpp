#include <thread>

#include "../../common/events/CollisionEvent.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/physics/VectorUtil.hpp"
#include "../../common/util/StateDef.hpp"
#include "ClientGameController.hpp"
#include <SFML/Graphics.hpp>

ClientGameController::ClientGameController(size_t max_player_count, size_t max_mine_count,
                                           ClientInputKeys keys)
    : GameController(max_player_count, max_mine_count), m_networkingClient(new NetworkingClient()),
      m_animationController(new AnimationController()), m_clientInputKeys(keys) {
    addEventListeners();
}

ClientGameController::~ClientGameController() {}

void ClientGameController::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_COLLISION,
        std::bind(&ClientGameController::handleCollisionEvent, this, std::placeholders::_1));
}

PlayerInputs ClientGameController::collectInputs() {
    return getClientInputs(m_clientReliableUpdate, m_clientUnreliableUpdate);
}

void ClientGameController::setNetworkState() {
    // noop
}

void ClientGameController::incrementTick() { m_networkingClient->incrementTick(); }

void ClientGameController::updateView(sf::RenderWindow& window,
                                      sf::Vector2f buffer_scaling_factor) {
    // Update view to match player's group's position
    sf::Vector2f group_position =
        m_gameObjectStore->getGroup(m_groupController->getGroupId(m_playerId))->getCenter();
    sf::View view = window.getView();
    sf::Vector2f group_view_coordinates = {group_position.x * buffer_scaling_factor.x,
                                           group_position.y * buffer_scaling_factor.y};
    view.setCenter(group_view_coordinates);
    window.setView(view);
}

void ClientGameController::draw(sf::RenderTarget& target) {
    m_groupController->draw(target);
    m_mineController->draw(target);
    m_animationController->draw(target);
}

void ClientGameController::handleEvents(sf::RenderWindow& window) {
    m_clientReliableUpdate.setAll(false);
    sf::Vector2f direction = m_clientUnreliableUpdate.direction;

    // Process events
    sf::Event event;
    while (window.pollEvent(event)) {
        // Close window: exit
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        // Handle game controller events (e.g. player input)
        if (event.type == sf::Event::KeyPressed) {
            if (sf::Keyboard::isKeyPressed(m_clientInputKeys.joinable)) {
                m_clientReliableUpdate.toggle_joinable = true;
            }
            if (sf::Keyboard::isKeyPressed(m_clientInputKeys.ungroup)) {
                m_clientReliableUpdate.toggle_ungroup = true;
            }
            // TODO(sourenp|#103): Send key toggles instead of direction value
            if (sf::Keyboard::isKeyPressed(m_clientInputKeys.up)) {
                direction += sf::Vector2f(0.f, -1.f);
            }
            if (sf::Keyboard::isKeyPressed(m_clientInputKeys.down)) {
                direction += sf::Vector2f(0.f, 1.f);
            }
            if (sf::Keyboard::isKeyPressed(m_clientInputKeys.left)) {
                direction += sf::Vector2f(-1.f, 0.f);
            }
            if (sf::Keyboard::isKeyPressed(m_clientInputKeys.right)) {
                direction += sf::Vector2f(1.f, 0.f);
            }
            direction = VectorUtil::normalize(direction);
            if (sf::Keyboard::isKeyPressed(m_clientInputKeys.stop)) {
                direction = sf::Vector2f(0.f, 0.f);
            }
        }
        m_clientUnreliableUpdate.direction = direction;
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

    if (m_networkingClient->getGameStateIsFresh()) {
        rewindAndReplay();
    }
    GameController::update();
}

void ClientGameController::step(std::shared_ptr<PlayerInputs> pi, sf::Int32 delta_ms) {
    computeGameState(pi, delta_ms);
    m_animationController->step(delta_ms);
}

void ClientGameController::rewindAndReplay() {
    // Rewind
    GameState game_state = m_networkingClient->getGameState();
    applyGameState(game_state);

    // Replay
    int tick_delta = getTick() - game_state.tick;
    if (tick_delta <= 0) {
        return;
    } // If the client is behind the server we don't need to replay

    // TODO(sourenp|#102): I think we never reach here. Might be a bug.

    // Loop through ticks that need to be replayed and apply client input from cache if present
    for (int i = 0; i < tick_delta; ++i) {
        ClientInputAndTick client_input_and_tick;
        unsigned int replay_tick = game_state.tick + i;

        if (m_tickToInput.count(replay_tick) > 0) {
            client_input_and_tick = m_tickToInput[replay_tick];
            auto pi = std::shared_ptr<PlayerInputs>(new PlayerInputs(
                getClientInputs(client_input_and_tick.cru, client_input_and_tick.cuu)));
            GameController::computeGameState(pi, GameController::MIN_TIME_STEP);
        } else {
            // If we don't have input for this tick pass in empty PlayerInputs
            auto pi = std::shared_ptr<PlayerInputs>(new PlayerInputs());
            GameController::computeGameState(pi, GameController::MIN_TIME_STEP);
        }
    }
    m_tickToInput.clear();
}

void ClientGameController::fetchPlayerId() {
    if (!m_playerIdAvailable) {
        std::tie(m_playerIdAvailable, m_playerId) = m_networkingClient->getPlayerId();
    }
}

void ClientGameController::setClientUpdates() {
    // Set input to send to server
    m_networkingClient->setClientUnreliableUpdate(m_clientUnreliableUpdate);
    if (!m_clientReliableUpdate.allFalse()) {
        m_networkingClient->setClientReliableUpdate(m_clientReliableUpdate);
    }

    // Save input and state for replay
    m_tickToInput[m_networkingClient->getTick()] = (ClientInputAndTick){
        m_clientUnreliableUpdate, m_clientReliableUpdate, m_networkingClient->getTick()};
}

PlayerInputs& ClientGameController::getClientInputs(ClientReliableUpdate cru,
                                                    ClientUnreliableUpdate cuu) {
    // If we don't know our player id then we can't interpolate our moves
    if (!m_playerIdAvailable) {
        return m_playerInputs;
    }

    m_playerInputs.player_reliable_updates.clear();
    m_playerInputs.player_reliable_updates.clear();

    PlayerReliableUpdate player_reliable_update = {
        .player_id = m_playerId,
        .client_reliable_update = cru,
    };

    PlayerUnreliableUpdate player_unreliable_update = {
        .player_id = m_playerId,
        .client_unreliable_update = cuu,
    };

    m_playerInputs.player_reliable_updates.push_back(player_reliable_update);
    m_playerInputs.player_unreliable_updates.push_back(player_unreliable_update);

    return m_playerInputs;
}

void ClientGameController::handleCollisionEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<CollisionEvent> collision_event =
        std::dynamic_pointer_cast<CollisionEvent>(event);

    createCollisionAnimation(collision_event->getCollision().position);
}

void ClientGameController::createCollisionAnimation(const sf::Vector2f& collision_position) {
    auto collision_sprite = std::unique_ptr<AnimatedSprite>(new AnimatedSprite(
        *m_resourceStore->getTexture("collision"), {6, 1}, 240, collision_position, {2.f, 2.f}));
    m_animationController->add(std::move(collision_sprite));
}

unsigned int ClientGameController::getTick() { return m_networkingClient->getTick(); }

void ClientGameController::setTick(unsigned int tick) { m_networkingClient->setTick(tick); }
