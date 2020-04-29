#include "ServerGameController.hpp"

ServerGameController::ServerGameController(LevelKey level_key, uint32_t tcp_port) :
    GameController(level_key), m_networkingServer(new NetworkingServer(tcp_port)) {
}

ServerGameController::~ServerGameController() {
}

void ServerGameController::draw() {
    auto player_ids_to_updates_rates = m_networkingServer->getPlayerUnreliableUpdatesRates();
    auto player_ids_to_avg_tick_drifts = m_networkingServer->getPlayerTickDrifts();
    auto broadcast_game_state_rate = m_networkingServer->getBroadcastGameStateRate();
    GameStateObject gso = m_gameObjectController->getGameStateObject();
    auto player_updates = gso.player_updates;
    m_terminalRenderingController.draw(
        player_ids_to_updates_rates, player_ids_to_avg_tick_drifts, broadcast_game_state_rate,
        player_updates, m_gameStepMetric.getRate(sf::seconds(1)),
        m_gameUpdateMetric.getRate(sf::seconds(1)), m_networkingServer->getTick());
}

void ServerGameController::start() {
    m_gameStateCore.status = GameStatus::playing;
    while (true) {
        step();
        draw();
    }
}

InputDef::PlayerInputs ServerGameController::getPlayerInputs() {
    return m_networkingServer->collectClientInputs();
}

void ServerGameController::preUpdate() {
    m_terminalRenderingController.preUpdate();
}

void ServerGameController::update(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) {
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
}

void ServerGameController::postUpdate(sf::Int32 update_time) {

    switch (m_gameStateCore.status) {
        case GameStatus::not_started: {
            // noop
            break;
        }
        case GameStatus::playing: {
            bool game_over;
            uint32_t winner_player_id;
            std::tie(game_over, winner_player_id) = m_gameObjectController->getGameOver();
            if (game_over) {
                m_terminalRenderingController.addWinner(std::to_string(winner_player_id));
                m_gameStateCore.status = GameStatus::game_over;
                m_gameStateCore.winner_player_id = winner_player_id;
            }
            m_gameStateCore.tick = m_networkingServer->getTick();
            setNetworkState();
            break;
        }
        case GameStatus::game_over: {
            // noop
            break;
        }
    }
}

void ServerGameController::setNetworkState() {
    GameState game_state = {
        .object = m_gameObjectController->getGameStateObject(),
        .core = m_gameStateCore,
    };
    m_networkingServer->setState(game_state);
}

void ServerGameController::incrementTick() {
    m_networkingServer->incrementTick();
}
