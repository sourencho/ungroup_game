#include <iostream>
#include <numeric>
#include <thread>

#include <SFML/Graphics.hpp>

#include "../../common/physics/VectorUtil.hpp"
#include "../../common/util/InputDef.hpp"
#include "../../common/util/StateDef.hpp"
#include "../../common/util/game_settings.hpp"
#include "ClientGameController.hpp"

ClientGameController::ClientGameController(bool is_headless, bool is_bot, BotStrategy strategy,
                                           const std::string& server_ip, uint32_t server_tcp_port, LevelKey level_key) :
    m_headless(is_headless),
    m_isBot(is_bot), m_strategy(strategy), m_serverIP(server_ip), GameController(level_key),
    m_networkingClient(new NetworkingClient(m_serverIP, server_tcp_port)) {
}

ClientGameController::~ClientGameController() {
}

void ClientGameController::start() {
    m_gameStateCore.status = GameStatus::not_started;
    registerClient();
    postRegisterInit();
    while (m_window.isOpen()) {
        step();
        draw();
        if (m_headless) {
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

void ClientGameController::postRegisterInit() {
    m_window.create(
        sf::VideoMode(RenderingDef::WINDOW_RESOLUTION.x, RenderingDef::WINDOW_RESOLUTION.y),
        "Ungroup", sf::Style::Close);
    m_renderingController = std::unique_ptr<RenderingController>(new RenderingController(
        m_window, *m_gameObjectController, *m_gameObjectStore, m_playerId, m_headless));
    m_inputController = std::unique_ptr<InputController>(new InputController(INPUT_KEYS));
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

    switch (m_gameStateCore.status) {
        case GameStatus::not_started: {
            // Keep fetching game state to check if game status changed from not_started
            GameState game_state = m_networkingClient->getLatestGameState();
            m_gameStateCore = game_state.core;
            break;
        }
        case GameStatus::playing: {
            if (m_isBot) {
                // Override user input with bot input
                inputs = m_gameObjectController->getBotMove(m_playerId, m_strategy);
            }
            sendInputs(inputs);
            saveInputs(inputs);
            rewindAndReplay();
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
    m_networkUpdateMetric.update();
    m_tickDeltaMetric.update();
}

void ClientGameController::postUpdate() {
    UIData ui_data = {
        .game_steps_per_second = m_gameStepMetric.getRate(sf::seconds(1)),
        .game_updates_per_second = m_gameUpdateMetric.getRate(sf::seconds(1)),
        .network_updates_per_second = m_networkUpdateMetric.getRate(sf::seconds(1)),
        .tick_delta_average = m_tickDeltaMetric.getAverage(),
        .resources = m_gameObjectController->getPlayerResources(m_playerId),
        .resource_goals = m_gameObjectController->getPlayerController()
                              .getPlayer(m_playerId)
                              ->getWinCondition()
                              .getResourceCountsToWin(),
        .game_status = m_gameStateCore.status,
        .winner_player_id = m_gameStateCore.winner_player_id,
        .tick = getTick(),
    };
    m_renderingController->postUpdate(ui_data);
}

void ClientGameController::rewindAndReplay() {
    uint32_t m_newGameStateTick = m_networkingClient->getLatestGameStateTick();
    if (m_newGameStateTick > m_largestAppliedGameStateTick) {
        GameState game_state = m_networkingClient->getLatestGameState();
        uint32_t client_tick = getTick();
        uint32_t server_tick = game_state.core.tick;
        int tick_delta = client_tick - server_tick;

        applyGameState(game_state);
        if (GAME_SETTINGS.REPLAY) {
            replay(client_tick, server_tick);
        }

        m_largestAppliedGameStateTick = m_newGameStateTick;
        m_tickDeltaMetric.pushCount(tick_delta);
        m_networkUpdateMetric.pushCount();
    } else if (m_newGameStateTick < m_largestAppliedGameStateTick) {
        std::cout << "Recieved stale game state: " << m_newGameStateTick << "<"
                  << m_largestAppliedGameStateTick << std::endl;
    }
}

void ClientGameController::applyGameState(GameState& game_state) {
    // Rewind
    m_gameObjectController->applyGameStateObject(game_state.object);
    m_gameStateCore = game_state.core;
    setTick(game_state.core.tick);
}

void ClientGameController::replay(uint32_t client_tick, uint32_t server_tick) {
    int tick_delta = client_tick - server_tick;

    if (tick_delta <= 0) {
        return;
    }

    // Prevent the client from drifting too far from server
    tick_delta = std::min(tick_delta, GAME_SETTINGS.REPLAY_TICK_DELTA_THRESHOLD);

    // Loop through ticks that need to be replayed and apply client input from cache if present
    for (int i = 0; i < tick_delta; ++i) {
        uint32_t replay_tick = server_tick + i;
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

    // Remove cached ticks older than the applied server game state's tick. We won't need them for
    // future replays because we never apply stale game states.
    for (auto it = m_tickToInput.begin(); it != m_tickToInput.end();) {
        if (it->first < server_tick) {
            m_tickToInput.erase(it++);
        } else {
            ++it;
        }
    }
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

uint32_t ClientGameController::getTick() {
    return m_networkingClient->getTick();
}

void ClientGameController::setTick(uint32_t tick) {
    m_networkingClient->setTick(tick);
}
