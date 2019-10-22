#include "EventController.hpp"

#include <iostream>

EventController::EventController() {}

void EventController::addEventListener(EventType event_type, EventCallback event_callback) {
    mEventMapLock.lock();
    mEventCallbackMap[event_type].push_back(event_callback);
    mEventMapLock.unlock();
}

void EventController::queueEvent(std::shared_ptr<Event> event) {
    mEventQueueLock.lock();
    mEventQueue.push(event);
    mEventQueueLock.unlock();
}

void EventController::forceQueueEvent(std::shared_ptr<Event> event) { mEventQueue.push(event); }

void EventController::forceProcessEvents() {
    while (!mEventQueue.empty()) {
        std::shared_ptr<Event> event = mEventQueue.front();
        for (const EventCallback event_callback : mEventCallbackMap[event->getType()]) {
            (event_callback)(event);
        }
        mEventQueue.pop();
    }
}

void EventController::lock() {
    mEventQueueLock.lock();
    mEventMapLock.lock();
}

void EventController::unlock() {
    mEventQueueLock.unlock();
    mEventMapLock.unlock();
}

void EventController::reset() {
    lock();
    mEventCallbackMap.clear();
    mEventQueue = {};
    unlock();
}

std::mutex EventController::mEventQueueLock;
std::mutex EventController::mEventMapLock;
EventController::EventTypeToCallbacks EventController::mEventCallbackMap;
EventController::EventQueue EventController::mEventQueue;
