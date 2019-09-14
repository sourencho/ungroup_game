#include "../../vendor/catch.hpp"
#include "../../../common/events/EventController.hpp"
#include "../../../common/events/Event.hpp"
#include "../../../testing/TestEvent.hpp"

int THE_GLOBAL = 0;

void add_to_the_global(std::shared_ptr<Event> event) {
    std::shared_ptr<TestEvent> test_event = std::dynamic_pointer_cast<TestEvent>(event);
    THE_GLOBAL += test_event->getNumber();
}

SCENARIO("EventController processes an event", "[events]") {
    GIVEN("an event listener is added") {
        std::function<void(std::shared_ptr<Event>)> add_to_the_global_ptr = add_to_the_global;
        EventController::getInstance().addEventListener(EventType::EVENT_TYPE_TEST,
            add_to_the_global_ptr);

        WHEN("an event is queued") {
            std::shared_ptr<TestEvent> test_event = std::shared_ptr<TestEvent>(new TestEvent(1));
            EventController::getInstance().queueEvent(test_event);

            WHEN("events are processed") {
                EventController::getInstance().forceProcessEvents();
                REQUIRE(THE_GLOBAL == 1);
            }
        }
    }
}
