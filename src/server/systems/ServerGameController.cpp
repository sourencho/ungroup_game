#include "ServerGameController.hpp"

#include "../../common/events/EventController.hpp"
#include "../../common/events/ClientConnectedEvent.hpp"
#include "../../common/events/ClientDisconnectedEvent.hpp"


ServerGameController::ServerGameController(size_t max_player_count, size_t max_mine_count):
  GameController(max_player_count, max_mine_count), mNetworkingServer(new NetworkingServer()) {
    EventController::getInstance().addEventListener(EventType::EVENT_TYPE_CLIENT_CONNECTED,
        std::bind(&ServerGameController::clientConnected, this, std::placeholders::_1));
    EventController::getInstance().addEventListener(EventType::EVENT_TYPE_CLIENT_DISCONNECTED,
        std::bind(&ServerGameController::clientDisconnected, this, std::placeholders::_1));
}

ServerGameController::~ServerGameController() {}

ClientInputs ServerGameController::collectInputs() {
    return mNetworkingServer->collectClientInputs();
}

void ServerGameController::clientConnected(std::shared_ptr<Event> event) {
    switch (event->getType()) {
        case EventType::EVENT_TYPE_CLIENT_CONNECTED: {
            std::shared_ptr<ClientConnectedEvent> client_connect_event = \
                std::dynamic_pointer_cast<ClientConnectedEvent>(event);
            uint32_t new_client_id = client_connect_event->getClientId();
            uint32_t new_player_id = createPlayerWithGroup(new_client_id);
            mNetworkingServer->setClientToPlayerId(new_client_id, new_player_id);
            break;
        }
        default: {
            std::cout << "Unexpected event type." << std::endl;
            break;
        }
    }
}


void ServerGameController::clientDisconnected(std::shared_ptr<Event> event) {
    switch (event->getType()) {
        case EventType::EVENT_TYPE_CLIENT_DISCONNECTED: {
            std::shared_ptr<ClientDisconnectedEvent> client_disconnect_event = \
                std::dynamic_pointer_cast<ClientDisconnectedEvent>(event);
            uint32_t removed_client_id = client_disconnect_event->getClientId();
            mPlayerController->removePlayer(removed_client_id);
            break;
        }
        default: {
            std::cout << "Unexpected event type." << std::endl;
            break;
        }
    }
}

void ServerGameController::setNetworkState() {
    mNetworkingServer->setState(GameController::getGameState());
}

void ServerGameController::incrementTick() {
    mNetworkingServer->incrementTick();
}

unsigned int ServerGameController::getTick() {
    return mNetworkingServer->getTick();
}

void ServerGameController::setTick(unsigned int tick) {
    mNetworkingServer->setTick(tick);
}
