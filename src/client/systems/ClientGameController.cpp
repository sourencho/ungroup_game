#include <thread>

#include "../../common/events/CollisionEvent.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/physics/VectorUtil.hpp"
#include "../../common/util/StateDef.hpp"
#include "../../common/util/game_settings.hpp"
#include "ClientGameController.hpp"
#include <SFML/Graphics.hpp>

ClientGameController::ClientGameController(InputDef::InputKeys keys, sf::RenderWindow& window,
                                           sf::RenderTexture& buffer,
                                           sf::Vector2f buffer_scaling_factor,
                                           sf::Sprite& buffer_sprite) :
    GameController(),
    m_networkingClient(new NetworkingClient()), m_animationController(new AnimationController()),
    m_inputController(new InputController(keys)), m_window(window), m_buffer(buffer),
    m_bufferSprite(buffer_sprite), m_bufferScalingFactor(buffer_scaling_factor) {
    addEventListeners();
}

ClientGameController::~ClientGameController() {
}

void ClientGameController::start() {
    registerClient();
    while (m_window.isOpen()) {
        step();
        draw();
    }
}

void ClientGameController::registerClient() {
    std::cout << "Registering client with server..." << std::endl;
    m_playerId = m_networkingClient->registerClientAndFetchPlayerId();
}

void ClientGameController::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_COLLISION,
        std::bind(&ClientGameController::handleCollisionEvent, this, std::placeholders::_1));
}

void ClientGameController::incrementTick() {
    m_networkingClient->incrementTick();
}

void ClientGameController::updateView() {
    // Update view to match player's group's position
    sf::Vector2f player_position = m_gameObjectController->getPlayerPosition(m_playerId);
    sf::View view = m_window.getView();
    sf::Vector2f group_view_coordinates = {player_position.x * m_bufferScalingFactor.x,
                                           player_position.y * m_bufferScalingFactor.y};
    view.setCenter(group_view_coordinates);
    m_window.setView(view);
}

void ClientGameController::draw() {
    updateView();

    m_window.clear(sf::Color::White);
    m_buffer.clear(BACKGROUND_COLOR);

    m_gameObjectController->draw(m_buffer);
    m_animationController->draw(m_buffer);

    m_buffer.display();
    m_window.draw(m_bufferSprite);
    m_window.display();
}

InputDef::PlayerInputs ClientGameController::getPlayerInputs() {
    return m_inputController->getPlayerInputs(m_playerId);
}

void ClientGameController::preUpdate() {
    auto inputs = m_inputController->collectInputs(m_window);
    sendInputs(inputs);
    saveInputs(inputs);

    if (m_networkingClient->getGameStateIsFresh()) {
        rewindAndReplay();
    }
}

void ClientGameController::update(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) {
    computeGameState(pi, delta_ms);
    m_animationController->step(delta_ms);
}

void ClientGameController::postUpdate() {
    // noop
}

/*
 * Rewind and replay is applying the game state update from the server (rewind because it is likely
 * for an old tick) and then interpolating up to the current tick via interpolation (replay).
 */
void ClientGameController::rewindAndReplay() {
    // Rewind
    GameState game_state = m_networkingClient->getGameState();
    setTick(static_cast<unsigned int>(game_state.tick));
    m_gameObjectController->applyGameState(game_state);

    // Replay
    int tick_delta = getTick() - game_state.tick;
    if (tick_delta <= 0) {
        return;
    } // If the client is behind the server we don't need to replay

    // TODO(sourenp|#102): I think we never reach here. Might be a bug.

    // Loop through ticks that need to be replayed and apply client input from cache if present
    for (int i = 0; i < tick_delta; ++i) {
        unsigned int replay_tick = game_state.tick + i;
        if (m_tickToInput.count(replay_tick) > 0) {
            InputDef::ClientInputAndTick client_input_and_tick = m_tickToInput[replay_tick];
            auto pi = InputDef::PlayerInputs(m_inputController->getPlayerInputs(
                m_playerId, client_input_and_tick.ri, client_input_and_tick.ui));
            GameController::computeGameState(pi, GameController::MIN_TIME_STEP);
        } else {
            // If we don't have input for this tick pass in empty inputs
            auto pi = InputDef::PlayerInputs();
            GameController::computeGameState(pi, GameController::MIN_TIME_STEP);
        }
    }
    m_tickToInput.clear();
}

/**
 * Send client input to server
 */
void ClientGameController::sendInputs(
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> inputs) {
    if (!inputs.first.allFalse()) {
        m_networkingClient->pushReliableInput(inputs.first);
    }
    if (!inputs.second.allFalse()) {
        m_networkingClient->pushUnreliableInput(inputs.second);
    }
}

/**
 * Save input and state for replay
 */
void ClientGameController::saveInputs(
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> inputs) {
    m_tickToInput[m_networkingClient->getTick()] =
        (InputDef::ClientInputAndTick){inputs.second, inputs.first, m_networkingClient->getTick()};
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

unsigned int ClientGameController::getTick() {
    return m_networkingClient->getTick();
}

void ClientGameController::setTick(unsigned int tick) {
    m_networkingClient->setTick(tick);
}
