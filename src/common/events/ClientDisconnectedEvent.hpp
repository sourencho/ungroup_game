#ifndef ClientDisconnectedEvent_hpp
#define ClientDisconnectedEvent_hpp

#include "Event.hpp"

class ClientDisconnectedEvent : public Event {
  public:
    explicit ClientDisconnectedEvent(uint32_t client_id, uint32_t player_id)
        : Event(EventType::EVENT_TYPE_CLIENT_DISCONNECTED), m_clientId(client_id),
          m_playerId(player_id){};

    uint32_t getClientId() { return m_clientId; };
    uint32_t getPlayerId() { return m_playerId; };

  private:
    uint32_t m_clientId;
    uint32_t m_playerId;
};

#endif /* ClientDisconnectedEvent_hpp */
