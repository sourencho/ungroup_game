#include "ServerGameController.hpp"

ServerGameController::ServerGameController() :
    GameController(), m_networkingServer(new NetworkingServer()) {
}

ServerGameController::~ServerGameController() {
}

void ServerGameController::start() {
    m_state = State::playing;
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
    if (m_gameObjectController->getGameOver()) {
        m_state = State::game_over;
    }
    setNetworkState();
}

void ServerGameController::setNetworkState() {
    m_networkingServer->setState(m_gameObjectController->getGameState(getTick(), m_state));
}

void ServerGameController::incrementTick() {
    m_networkingServer->incrementTick();
}

unsigned int ServerGameController::getTick() {
    return m_networkingServer->getTick();
}

void ServerGameController::setTick(unsigned int tick) {
    m_networkingServer->setTick(tick);
}
