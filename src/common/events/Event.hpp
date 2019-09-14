#ifndef Event_hpp
#define Event_hpp

enum EventType {
    EVENT_TYPE_TEST,
    EVENT_TYPE_GROUP_COLLISION,
    EVENT_TYPE_CLIENT_CONNECTED,
    EVENT_TYPE_CLIENT_DISCONNECTED
};

class Event {
 public:
    explicit Event(EventType event_type);

    virtual const EventType getType();
 private:
    EventType mEventType;
};

#endif /* Event_hpp */
