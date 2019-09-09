#include "EventController.hpp"

#include <iostream>


EventController::EventController() {}

void EventController::addEventListener(EventType event_type, EventCallback event_callback) {
    mEventCallbackMap[event_type].push_back(event_callback);
}

void EventController::queueEvent(std::shared_ptr<Event> event) {
    mEventQueue.push(event);
}

void EventController::processEvents() {
    while (!mEventQueue.empty()) {
        std::shared_ptr<Event> event = mEventQueue.front();

        for (const EventCallback event_callback : mEventCallbackMap[event->getType()]) {
            (event_callback)(event);
        }

        mEventQueue.pop();
    }
}

EventController* EventController::mInstance;
EventController::EventTypeToCallbacks EventController::mEventCallbackMap;
EventController::EventQueue EventController::mEventQueue;
