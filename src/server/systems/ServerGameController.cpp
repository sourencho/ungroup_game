#include "ServerGameController.hpp"

ServerGameController::ServerGameController() :
    GameController(), m_networkingServer(new NetworkingServer()) {
}

ServerGameController::~ServerGameController() {
}

void ServerGameController::start() {
    m_gameStateCore.status = GameStatus::playing;
    while (true) {
        step();
    }
}

InputDef::PlayerInputs ServerGameController::getPlayerInputs() {
    return m_networkingServer->collectClientInputs();
}

void ServerGameController::preUpdate() {
    // noop
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

void ServerGameController::postUpdate() {

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