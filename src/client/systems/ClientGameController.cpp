#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>

#include "../../common/physics/VectorUtil.hpp"
#include "../../common/util/InputDef.hpp"
#include "../../common/util/StateDef.hpp"
#include "../../common/util/game_settings.hpp"
#include "ClientGameController.hpp"

ClientGameController::ClientGameController() :
    GameController(),
    m_window(sf::VideoMode(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y), "Ungroup", sf::Style::Close),
    m_networkingClient(new NetworkingClient()), m_inputController(new InputController(INPUT_KEYS)),
    m_renderingController(
        new RenderingController(m_window, *m_gameObjectController, *m_resourceStore)) {
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

void ClientGameController::incrementTick() {
    m_networkingClient->incrementTick();
}

void ClientGameController::draw() {
    m_renderingController->draw();
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

    m_renderingController->preUpdate();
}

void ClientGameController::update(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) {
    computeGameState(pi, delta_ms);
    m_renderingController->update(delta_ms);
}

void ClientGameController::postUpdate() {
    sf::Vector2f player_position = m_gameObjectController->getPlayerPosition(m_playerId);
    UIData ui_data = {
        .frame_rate =
            static_cast<float>(m_frameCount) / (static_cast<float>(m_elapsedTime) / 1000.f),
        .resources = m_gameObjectController->getPlayerResources(m_playerId),
    };
    m_renderingController->postUpdate(player_position, ui_data);
}

void ClientGameController::rewindAndReplay() {
    GameState game_state = m_networkingClient->getGameState();
    int client_tick = getTick();
    int server_tick = game_state.tick;
    int tick_delta = client_tick - server_tick;

    // Rewind
    m_gameObjectController->applyGameState(game_state);
    setTick(static_cast<unsigned int>(server_tick));

    // Replay
    if (!USE_INTERPOLATION_REPLAY || tick_delta <= 0) {
        return;
    } // If the client is behind the server we don't need to replay

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

void ClientGameController::sendInputs(
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> inputs) {
    if (!inputs.first.allFalse()) {
        m_networkingClient->pushReliableInput(inputs.first);
    }
    if (!inputs.second.allFalse()) {
        m_networkingClient->pushUnreliableInput(inputs.second);
    }
}

void ClientGameController::saveInputs(
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> inputs) {
    m_tickToInput[m_networkingClient->getTick()] =
        (InputDef::ClientInputAndTick){inputs.second, inputs.first, m_networkingClient->getTick()};
}

unsigned int ClientGameController::getTick() {
    return m_networkingClient->getTick();
}

void ClientGameController::setTick(unsigned int tick) {
    m_networkingClient->setTick(tick);
}
