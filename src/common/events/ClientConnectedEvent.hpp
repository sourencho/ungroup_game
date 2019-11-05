#ifndef ClientConnectedEvent_hpp
#define ClientConnectedEvent_hpp

#include "Event.hpp"

class ClientConnectedEvent : public Event {
  public:
    explicit ClientConnectedEvent(uint32_t client_id)
        : Event(EventType::EVENT_TYPE_CLIENT_CONNECTED), mClientId(client_id){};

    uint32_t getClientId() { return mClientId; };

  private:
    uint32_t mClientId;
};

#endif /* ClientConnectedEvent_hpp */
