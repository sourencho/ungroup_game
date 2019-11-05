#ifndef ClientDisconnectedEvent_hpp
#define ClientDisconnectedEvent_hpp

#include "Event.hpp"

class ClientDisconnectedEvent : public Event {
  public:
    explicit ClientDisconnectedEvent(uint32_t client_id, uint32_t player_id)
        : Event(EventType::EVENT_TYPE_CLIENT_DISCONNECTED), mClientId(client_id),
          mPlayerId(player_id){};

    uint32_t getClientId() { return mClientId; };
    uint32_t getPlayerId() { return mPlayerId; };

  private:
    uint32_t mClientId;
    uint32_t mPlayerId;
};

#endif /* ClientDisconnectedEvent_hpp */
