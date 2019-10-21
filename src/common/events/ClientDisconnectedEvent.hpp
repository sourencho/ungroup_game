#ifndef ClientDisconnectedEvent_hpp
#define ClientDisconnectedEvent_hpp

#include "Event.hpp"

class ClientDisconnectedEvent : public Event {
  public:
    explicit ClientDisconnectedEvent(int client_id)
        : Event(EventType::EVENT_TYPE_CLIENT_DISCONNECTED), mClientId(client_id){};

    int getClientId() { return mClientId; };

  private:
    int mClientId = -1;
};

#endif /* ClientDisconnectedEvent_hpp */
