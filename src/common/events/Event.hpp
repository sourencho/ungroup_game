#ifndef Event_hpp
#define Event_hpp

enum EventType {
    EVENT_TYPE_GROUP_COLLISION
};

class Event {
 public:
    explicit Event(EventType event_type);

    EventType getType();
 private:
    EventType mEventType;
};

#endif /* Event_hpp */
