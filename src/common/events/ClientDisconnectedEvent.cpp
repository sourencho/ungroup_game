#include "ClientDisconnectedEvent.hpp"

ClientDisconnectedEvent::ClientDisconnectedEvent(int client_id)
    : Event(EventType::EVENT_TYPE_CLIENT_DISCONNECTED) {
    mClientId = client_id;
}

int ClientDisconnectedEvent::getClientId() { return mClientId; }
