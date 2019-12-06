#include "ServerGameController.hpp"

ServerGameController::ServerGameController() :
    GameController(), m_networkingServer(new NetworkingServer()) {
}

ServerGameController::~ServerGameController() {
}

void ServerGameController::start() {
    m_gameCoreState.status = GameStatus::playing;
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
    computeGameState(pi, delta_ms);
}

void ServerGameController::postUpdate() {
    bool game_over;
    uint32_t winner_player_id;
    std::tie(game_over, winner_player_id) = m_gameObjectController->getGameOver();
    if (game_over) {
        m_gameCoreState.status = GameStatus::game_over;
        m_gameCoreState.winner_player_id = winner_player_id;
    }
    m_gameCoreState.tick = m_networkingServer->getTick();
    setNetworkState();
}

void ServerGameController::setNetworkState() {
    GameState game_state = {
        .object = m_gameObjectController->getGameStateObject(),
        .core = m_gameCoreState,
    };
    m_networkingServer->setState(game_state);
}

void ServerGameController::incrementTick() {
    m_networkingServer->incrementTick();
}