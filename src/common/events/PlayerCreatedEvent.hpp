#ifndef PlayerCreatedEvent_hpp
#define PlayerCreatedEvent_hpp

#include "Event.hpp"

class PlayerCreatedEvent : public Event {
  public:
    explicit PlayerCreatedEvent(uint32_t client_id, uint32_t player_id) :
        Event(EventType::EVENT_TYPE_PLAYER_CREATED), m_clientId(client_id), m_playerId(player_id){};

    uint32_t getClientId() {
        return m_clientId;
    };
    uint32_t getPlayerId() {
        return m_playerId;
    };

  private:
    uint32_t m_clientId;
    uint32_t m_playerId;
};

#endif /* PlayerCreatedEvent_hpp */
