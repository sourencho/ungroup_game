#include "EventController.hpp"

#include <iostream>


EventController::EventController() {}

void EventController::addEventListener(EventType event_type, EventCallback event_callback) {
    mEventCallbackMap[event_type].push_back(event_callback);
}

void EventController::queueEvent(Event* event) {
    mEventQueue.push(event);
}

void EventController::processEvents() {
    while (!mEventQueue.empty()) {
        Event* event = mEventQueue.front();
        std::cout << event->getType() << std::endl;

        for (const EventCallback event_callback : mEventCallbackMap[event->getType()]) {
            (event_callback)(event);
        }

        delete event;
        mEventQueue.pop();
    }
}

EventController* EventController::mInstance;
EventController::EventTypeToCallbacks EventController::mEventCallbackMap;
EventController::EventQueue EventController::mEventQueue;
