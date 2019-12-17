#include <iostream>
#include <thread>

#include <SFML/Graphics.hpp>
#include <thread>

#include "../../common/physics/VectorUtil.hpp"
#include "../../common/util/InputDef.hpp"
#include "../../common/util/StateDef.hpp"
#include "../../common/util/game_settings.hpp"
#include "ClientGameController.hpp"

ClientGameController::ClientGameController(bool is_headless, bool is_bot, BotStrategy strategy) :
    m_headless(is_headless), m_isBot(is_bot), m_strategy(strategy), GameController(),
    m_window(sf::VideoMode(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y), "Ungroup", sf::Style::Close),
    m_networkingClient(new NetworkingClient()), m_inputController(new InputController(INPUT_KEYS)),
    m_renderingController(
        new RenderingController(m_window, *m_gameObjectController, *m_resourceStore)) {
}

ClientGameController::~ClientGameController() {
}

void ClientGameController::start() {
    m_gameStateCore.status = GameStatus::not_started;
    registerClient();
    while (m_window.isOpen()) {
        step();
        if (!m_headless) {
            draw();
        } else {
            // unfortunately, the game will buffer up a ton of
            // non-relevant moves, unless we slow it down. previously
            // the draw was slowing it.
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
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
    // call collectinputs to process window events, but if we're in bot mode
    // then override any user commands
    auto inputs = m_inputController->collectInputs(m_window);

    switch (m_gameStateCore.status) {
        case GameStatus::not_started: {
            // Keep fetching game state to check if game status changed from not_started
            GameState game_state = m_networkingClient->getGameState();
            m_gameStateCore = game_state.core;
            break;
        }
        case GameStatus::playing: {
            if (m_isBot) {
                inputs = m_gameObjectController->getBotMove(m_playerId, m_strategy);
            }
            sendInputs(inputs);
            saveInputs(inputs);

            if (m_networkingClient->getGameStateIsFresh()) {
                rewindAndReplay();
            }
            break;
        }
        case GameStatus::game_over: {
            // noop
            break;
        }
    }

    m_renderingController->preUpdate();
}

void ClientGameController::update(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) {
    switch (m_gameStateCore.status) {
        case GameStatus::not_started: {
            // noop
            break;
        }
        case GameStatus::playing: {
            computeGameState(pi, delta_ms);
            break;
        }
        case GameStatus::game_over: {
            // noop
            break;
        }
    }

    m_renderingController->update(delta_ms);
}

void ClientGameController::postUpdate() {
    sf::Vector2f player_position = m_gameObjectController->getPlayerPosition(m_playerId);
    UIData ui_data = {
        .steps_per_second =
            static_cast<float>(m_stepCount) / (static_cast<float>(m_elapsedTime) / 1000.f),
        .updates_per_second =
            static_cast<float>(m_updateCount) / (static_cast<float>(m_elapsedTime) / 1000.f),
        .resources = m_gameObjectController->getPlayerResources(m_playerId),
        .game_status = m_gameStateCore.status,
        .winner_player_id = m_gameStateCore.winner_player_id,
    };
    m_renderingController->postUpdate(player_position, ui_data);
}

void ClientGameController::rewindAndReplay() {
    GameState game_state = m_networkingClient->getGameState();

    unsigned int client_tick = getTick();
    unsigned int server_tick = game_state.core.tick;
    int tick_delta = client_tick - server_tick;

    // Rewind
    m_gameObjectController->applyGameStateObject(game_state.object);
    m_gameStateCore = game_state.core;
    setTick(server_tick);

    // Replay
    if (!USE_INTERPOLATION_REPLAY || tick_delta <= 0) {
        return;
    } // If the client is behind the server we don't need to replay

    // Loop through ticks that need to be replayed and apply client input from cache if present
    for (int i = 0; i < tick_delta; ++i) {
        unsigned int replay_tick = server_tick + i;
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
