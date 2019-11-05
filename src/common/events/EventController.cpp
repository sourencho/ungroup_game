#include "EventController.hpp"

#include <iostream>

EventController::EventController() {}

void EventController::addEventListener(EventType event_type, EventCallback event_callback) {
    m_eventMapLock.lock();
    m_eventCallbackMap[event_type].push_back(event_callback);
    m_eventMapLock.unlock();
}

void EventController::queueEvent(std::shared_ptr<Event> event) {
    m_eventQueueLock.lock();
    m_eventQueue.push(event);
    m_eventQueueLock.unlock();
}

void EventController::forceQueueEvent(std::shared_ptr<Event> event) { m_eventQueue.push(event); }

void EventController::forceProcessEvents() {
    while (!m_eventQueue.empty()) {
        std::shared_ptr<Event> event = m_eventQueue.front();
        for (const EventCallback event_callback : m_eventCallbackMap[event->getType()]) {
            (event_callback)(event);
        }
        m_eventQueue.pop();
    }
}

void EventController::lock() {
    m_eventQueueLock.lock();
    m_eventMapLock.lock();
}

void EventController::unlock() {
    m_eventQueueLock.unlock();
    m_eventMapLock.unlock();
}

void EventController::reset() {
    lock();
    m_eventCallbackMap.clear();
    m_eventQueue = {};
    unlock();
}

std::mutex EventController::m_eventQueueLock;
std::mutex EventController::m_eventMapLock;
EventController::EventTypeToCallbacks EventController::m_eventCallbackMap;
EventController::EventQueue EventController::m_eventQueue;
