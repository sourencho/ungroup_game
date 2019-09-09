#ifndef EventController_hpp
#define EventController_hpp

#include <functional>
#include <list>
#include <map>
#include <queue>

#include "Event.hpp"


typedef std::function<void(Event*)> EventCallback;

class EventController {
 public:
    static EventController* getInstance() {
        if (!mInstance)
            mInstance = new EventController();

        return mInstance;
    }

    static void destroyInstance() {
        if (mInstance)
            delete mInstance;

        mInstance = 0;
    }

    void addEventListener(EventType event_type, EventCallback event_callback);
    void queueEvent(Event* event);
    void processEvents();

 private:
    typedef std::list<EventCallback> EventCallbackList;
    typedef std::map<EventType, EventCallbackList> EventTypeToCallbacks;
    typedef std::queue<Event*> EventQueue;

    EventController();

    static EventController* mInstance;
    static EventTypeToCallbacks mEventCallbackMap;
    static EventQueue mEventQueue;
};

#endif /* EventController_hpp */
