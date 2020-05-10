#include <cmath>
#include <iostream>
#include <numeric>
#include <thread>

#include <SFML/Graphics.hpp>

#include "../../common/events/CollisionEvent.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/physics/VectorUtil.hpp"
#include "../../common/util/InputDef.hpp"
#include "../../common/util/StateDef.hpp"
#include "../../common/util/game_settings.hpp"
#include "ClientGameController.hpp"

const uint MAX_BEHIND_TICK_DELTA = 10;

ClientGameController::ClientGameController(bool is_headless, bool is_bot, BotStrategy strategy,
                                           const std::string& server_ip, uint32_t server_tcp_port,
                                           LevelKey level_key) :
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

std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
ClientGameController::getBotMove(uint32_t bot_player_id, BotStrategy strategy) {
    return m_bot.getMove(bot_player_id, strategy, *m_gameObjectController);
}

void ClientGameController::preUpdate() {
    auto inputs = m_inputController->collectInputs(m_window);

    switch (m_gameStateCore.status) {
        case GameStatus::not_started: {
            // Keep fetching game state to check if game status changed from not_started
            std::map<uint32_t, GameState> game_state_buffer =
                m_networkingClient->getGameStateBuffer();
            if (game_state_buffer.size() == GAME_SETTINGS.CLIENT_GAME_STATE_BUFFER_SIZE) {
                m_gameStateCore = game_state_buffer.begin()->second.core;
            }
            break;
        }
        case GameStatus::playing: {
            if (m_isBot) {
                // Override user input with bot input
                inputs = getBotMove(m_playerId, m_strategy);
            }
            sendInputs(inputs);
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
            updateGameState(pi, delta_ms);
            break;
        }
        case GameStatus::game_over: {
            // noop
            break;
        }
    }

    m_renderingController->update(delta_ms);
    m_tickDeltaMetric.update();
    m_behindGameStateMetric.update();
    m_aheadGameStateMetric.update();
    m_interpolateGameStateMetric.update();
    m_stallGameStateMetric.update();
}

void ClientGameController::postUpdate(sf::Int32 update_time) {
    UIData ui_data = {
        .game_steps_per_second = m_gameStepMetric.getRate(sf::seconds(1)),
        .game_updates_per_second = m_gameUpdateMetric.getRate(sf::seconds(1)),
        .server_state_per_second = m_networkingClient->getServerGameStateRate(sf::seconds(1)),
        .tick_delta_average = m_tickDeltaMetric.getAverage(),
        .behind_game_state_rate = m_behindGameStateMetric.getRate(sf::seconds(1)),
        .ahead_game_state_rate = m_aheadGameStateMetric.getRate(sf::seconds(1)),
        .interpolate_distance_average = m_interpolateGameStateMetric.getAverage(),
        .stall_distance_average = m_stallGameStateMetric.getAverage(),
        .resources = m_gameObjectController->getPlayerResources(m_playerId),
        .resource_goals = m_gameObjectController->getPlayerController()
                              .getPlayer(m_playerId)
                              ->getWinCondition()
                              .getResourceCountsToWin(),
        .game_status = m_gameStateCore.status,
        .winner_player_id = m_gameStateCore.winner_player_id,
        .tick = getTick(),
    };
    m_renderingController->postUpdate(update_time, ui_data);
}

void ClientGameController::updateGameState(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) {

    std::map<uint32_t, GameState> game_state_buffer = m_networkingClient->getGameStateBuffer();

    if (game_state_buffer.empty()) {
        return;
    }

    uint32_t client_tick = getTick();
    uint32_t smallest_server_tick = game_state_buffer.begin()->first;
    const GameState& smallest_server_game_state = game_state_buffer.begin()->second;

    if (client_tick < smallest_server_tick) {
        behind(client_tick, smallest_server_tick, smallest_server_game_state);
        m_behindGameStateMetric.pushCount();
    } else {
        ahead(client_tick, smallest_server_tick, game_state_buffer);
        m_aheadGameStateMetric.pushCount();
    }
    m_tickDeltaMetric.pushCount(smallest_server_tick - client_tick);
}

/**
 * If client is too far behind smallest server game state in the buffer, jump to server
 * game state, otherwise interpolate the game state of the next tick.
 */
void ClientGameController::behind(uint32_t client_tick, uint32_t smallest_server_tick,
                                  const GameState& smallest_server_game_state) {
    uint32_t next_tick;
    if (smallest_server_tick - client_tick > MAX_BEHIND_TICK_DELTA) {
        next_tick = smallest_server_tick;
    } else {
        next_tick = client_tick + 1;
    }
    interpolateGameState(client_tick, next_tick, smallest_server_tick, smallest_server_game_state);
}

/**
 * If client is ahead of the smallest server game state in the buffer, stay at the same tick but
 * interpolate to the next available game state in the buffer. If there are none available just keep
 * the game state the same. Note that the tick stays the same in this case so that we fall back
 * behind to being behind the smallest server game state in the buffer.
 */
void ClientGameController::ahead(uint32_t client_tick, uint32_t smallest_server_tick,
                                 std::map<uint32_t, GameState>& game_state_buffer) {
    std::map<uint32_t, GameState>::iterator game_state_buffer_it = game_state_buffer.begin();
    ++game_state_buffer_it;
    while (game_state_buffer_it != game_state_buffer.end()) {
        uint32_t next_server_tick = game_state_buffer_it->first;
        if (client_tick < next_server_tick) {
            const GameState& next_server_game_state = game_state_buffer_it->second;
            uint32_t next_tick = client_tick;
            // Keep the same tick but interpolate to the next available server game state.
            interpolateGameState(client_tick - 1, next_tick, next_server_tick,
                                 next_server_game_state);
            m_stallGameStateMetric.pushCount(client_tick - smallest_server_tick);
            return;
        }
        ++game_state_buffer_it;
    }

    // If we reach here then we don't have any game states to interpolate to. Keep the same tick and
    // game state.
    m_stallGameStateMetric.pushCount(client_tick - smallest_server_tick);
    return;
}

void ClientGameController::interpolateGameState(uint32_t start_tick, uint32_t to_tick,
                                                uint32_t end_tick, const GameState& game_state) {
    if (to_tick < start_tick || end_tick < to_tick) {
        throw std::runtime_error("Can't interpolate to the past.");
    }

    float a = static_cast<float>(to_tick - start_tick) / static_cast<float>(end_tick - start_tick);
    m_gameStateCore = game_state.core;
    sf::Int32 delta_ms = MIN_TIME_STEP * (to_tick - start_tick);
    m_gameObjectController->interpolateGameStateObject(game_state.object, a, delta_ms);
    applyGameStateEvents(game_state.event);
    setTick(to_tick);
    m_interpolateGameStateMetric.pushCount(end_tick - to_tick);
}

void ClientGameController::applyGameStateEvents(const GameStateEvent& game_state_event) {
    for (const auto& collision : game_state_event.collisions) {
        EventController::getInstance().forceQueueEvent(
            std::move(std::unique_ptr<CollisionEvent>(new CollisionEvent(collision))));
    }
    EventController::getInstance().forceProcessEvents();
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

uint32_t ClientGameController::getTick() {
    return m_networkingClient->getTick();
}

void ClientGameController::setTick(uint32_t tick) {
    m_networkingClient->setTick(tick);
}
