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
    EventController* event_controller = EventController::getInstance();

    GIVEN("an event listener is added") {
        std::function<void(std::shared_ptr<Event>)> add_to_the_global_ptr = add_to_the_global;
        event_controller->addEventListener(EventType::EVENT_TYPE_TEST, add_to_the_global_ptr);

        WHEN("an event is queued") {
            std::shared_ptr<TestEvent> test_event = std::shared_ptr<TestEvent>(new TestEvent(1));
            event_controller->queueEvent(test_event);

            WHEN("events are processed") {
                event_controller->processEvents();
                REQUIRE(THE_GLOBAL == 1);
            }
        }
    }
}
