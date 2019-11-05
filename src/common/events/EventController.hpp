#ifndef EventController_hpp
#define EventController_hpp

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <queue>

#include "Event.hpp"

typedef std::function<void(std::shared_ptr<Event>)> EventCallback;

class EventController {
  public:
    static EventController& getInstance() {
        static EventController instance;
        return instance;
    }

    // Let's make sure we don't accidentally get copies of the singleton.
    EventController(EventController const&) = delete;
    void operator=(EventController const&) = delete;

    void addEventListener(EventType event_type, EventCallback event_callback);
    void queueEvent(std::shared_ptr<Event> event);
    void forceQueueEvent(std::shared_ptr<Event> event);
    void forceProcessEvents();
    void lock();
    void unlock();
    void reset();

  private:
    typedef std::list<EventCallback> EventCallbackList;
    typedef std::map<EventType, EventCallbackList> EventTypeToCallbacks;
    typedef std::queue<std::shared_ptr<Event>> EventQueue;

    EventController();

    static EventTypeToCallbacks m_EventCallbackMap;
    static EventQueue m_EventQueue;

    static std::mutex m_EventQueueLock;
    static std::mutex m_EventMapLock;
};

#endif /* EventController_hpp */
