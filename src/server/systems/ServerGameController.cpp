#include "ServerGameController.hpp"

ServerGameController::ServerGameController() :
    GameController(), m_networkingServer(new NetworkingServer()) {
}

ServerGameController::~ServerGameController() {
}

PlayerInputs ServerGameController::getPlayerInputs() {
    return m_networkingServer->collectClientInputs();
}

void ServerGameController::preUpdate() {
    // noop
}

void ServerGameController::update(const PlayerInputs& pi, sf::Int32 delta_ms) {
    computeGameState(pi, delta_ms);
}

void ServerGameController::postUpdate() {
    setNetworkState();
}

void ServerGameController::setNetworkState() {
    m_networkingServer->setState(m_gameObjectController->getGameState(getTick()));
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
