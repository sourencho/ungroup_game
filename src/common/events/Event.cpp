#include "Event.hpp"

Event::Event(EventType event_type) {
    mEventType = event_type;
}

EventType Event::getType() {
    return mEventType;
}
