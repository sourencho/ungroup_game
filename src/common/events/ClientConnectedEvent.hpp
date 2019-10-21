#ifndef ClientConnectedEvent_hpp
#define ClientConnectedEvent_hpp

#include "Event.hpp"

class ClientConnectedEvent : public Event {
  public:
    explicit ClientConnectedEvent(int client_id)
        : Event(EventType::EVENT_TYPE_CLIENT_CONNECTED), mClientId(client_id){};

    int getClientId() { return mClientId; };

  private:
    int mClientId = -1;
};

#endif /* ClientConnectedEvent_hpp */
