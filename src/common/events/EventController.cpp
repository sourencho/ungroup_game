#include "EventController.hpp"

#include <iostream>

EventController::EventController() {}

void EventController::addEventListener(EventType event_type, EventCallback event_callback) {
    m_EventMapLock.lock();
    m_EventCallbackMap[event_type].push_back(event_callback);
    m_EventMapLock.unlock();
}

void EventController::queueEvent(std::shared_ptr<Event> event) {
    m_EventQueueLock.lock();
    m_EventQueue.push(event);
    m_EventQueueLock.unlock();
}

void EventController::forceQueueEvent(std::shared_ptr<Event> event) { m_EventQueue.push(event); }

void EventController::forceProcessEvents() {
    while (!m_EventQueue.empty()) {
        std::shared_ptr<Event> event = m_EventQueue.front();
        for (const EventCallback event_callback : m_EventCallbackMap[event->getType()]) {
            (event_callback)(event);
        }
        m_EventQueue.pop();
    }
}

void EventController::lock() {
    m_EventQueueLock.lock();
    m_EventMapLock.lock();
}

void EventController::unlock() {
    m_EventQueueLock.unlock();
    m_EventMapLock.unlock();
}

void EventController::reset() {
    lock();
    m_EventCallbackMap.clear();
    m_EventQueue = {};
    unlock();
}

std::mutex EventController::m_EventQueueLock;
std::mutex EventController::m_EventMapLock;
EventController::EventTypeToCallbacks EventController::m_EventCallbackMap;
EventController::EventQueue EventController::m_EventQueue;
