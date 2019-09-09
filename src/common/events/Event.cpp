#include "Event.hpp"

Event::Event(EventType event_type) {
    mEventType = event_type;
}

const EventType Event::getType() {
    return mEventType;
}
