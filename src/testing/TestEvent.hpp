#ifndef TestEvent_hpp
#define TestEvent_hpp

#include "../../src/common/events/Event.hpp"


class TestEvent : public Event {
 public:
     explicit TestEvent(int number);

     int getNumber();
 private:
     int mNumber = -1;
};

#endif /* TestEvent_hpp */
