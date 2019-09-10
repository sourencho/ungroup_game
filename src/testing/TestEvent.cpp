#include "TestEvent.hpp"


TestEvent::TestEvent(int number):Event(EventType::EVENT_TYPE_TEST) {
    mNumber = number;
}

int TestEvent::getNumber() {
    return mNumber;
}
