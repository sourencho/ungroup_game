#ifndef ClientDisconnectedEvent_hpp
#define ClientDisconnectedEvent_hpp

#include "Event.hpp"

class ClientDisconnectedEvent : public Event {
  public:
    explicit ClientDisconnectedEvent(uint32_t client_id, uint32_t player_id)
        : Event(EventType::EVENT_TYPE_CLIENT_DISCONNECTED), m_ClientId(client_id),
          m_PlayerId(player_id){};

    uint32_t getClientId() { return m_ClientId; };
    uint32_t getPlayerId() { return m_PlayerId; };

  private:
    uint32_t m_ClientId;
    uint32_t m_PlayerId;
};

#endif /* ClientDisconnectedEvent_hpp */
