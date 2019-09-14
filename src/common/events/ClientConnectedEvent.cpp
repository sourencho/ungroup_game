#include "ClientConnectedEvent.hpp"


ClientConnectedEvent::ClientConnectedEvent(int client_id)
  :Event(EventType::EVENT_TYPE_CLIENT_CONNECTED) {
    mClientId = client_id;
}

int ClientConnectedEvent::getClientId() {
    return mClientId;
}
