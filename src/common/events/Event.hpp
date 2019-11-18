#ifndef Event_hpp
#define Event_hpp

enum EventType {
    EVENT_TYPE_TEST,
    EVENT_TYPE_GROUP_COLLISION,
    EVENT_TYPE_CLIENT_CONNECTED,
    EVENT_TYPE_CLIENT_DISCONNECTED,
    EVENT_TYPE_COLLISION,
    EVENT_TYPE_PLAYER_CREATED
};

class Event {
  public:
    explicit Event(EventType event_type) : m_eventType(event_type){};

    virtual const EventType getType() {
        return m_eventType;
    };

  private:
    EventType m_eventType;
};

#endif /* Event_hpp */
