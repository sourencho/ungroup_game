#include "ServerGameController.hpp"

#include "../../common/events/ClientConnectedEvent.hpp"
#include "../../common/events/EventController.hpp"

ServerGameController::ServerGameController(size_t max_player_count, size_t max_mine_count)
    : GameController(max_player_count, max_mine_count), m_NetworkingServer(new NetworkingServer()) {
    addEventListeners();
}

ServerGameController::~ServerGameController() {}

void ServerGameController::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_CLIENT_CONNECTED,
        std::bind(&ServerGameController::handleClientConnectedEvent, this, std::placeholders::_1));
}

ClientInputs ServerGameController::collectInputs() {
    return m_NetworkingServer->collectClientInputs();
}

void ServerGameController::step(const ClientInputs& cis, sf::Int32 delta_ms) {
    computeGameState(cis, delta_ms);
}

void ServerGameController::handleClientConnectedEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<ClientConnectedEvent> client_connect_event =
        std::dynamic_pointer_cast<ClientConnectedEvent>(event);
    uint32_t new_client_id = client_connect_event->getClientId();
    uint32_t new_player_id = createPlayerWithGroup(new_client_id);
    m_NetworkingServer->setClientToPlayerId(new_client_id, new_player_id);
}

void ServerGameController::setNetworkState() {
    m_NetworkingServer->setState(GameController::getGameState());
}

void ServerGameController::incrementTick() { m_NetworkingServer->incrementTick(); }

unsigned int ServerGameController::getTick() { return m_NetworkingServer->getTick(); }

void ServerGameController::setTick(unsigned int tick) { m_NetworkingServer->setTick(tick); }
