#ifndef Event_hpp
#define Event_hpp

enum EventType {
    EVENT_TYPE_TEST,
    EVENT_TYPE_GROUP_COLLISION,
    EVENT_TYPE_CLIENT_CONNECTED,
    EVENT_TYPE_CLIENT_DISCONNECTED,
    EVENT_TYPE_COLLISION
};

class Event {
  public:
    explicit Event(EventType event_type) : m_EventType(event_type){};

    virtual const EventType getType() { return m_EventType; };

  private:
    EventType m_EventType;
};

#endif /* Event_hpp */
