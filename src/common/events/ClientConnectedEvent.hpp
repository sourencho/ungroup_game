#ifndef ClientConnectedEvent_hpp
#define ClientConnectedEvent_hpp

#include "Event.hpp"


class ClientConnectedEvent : public Event {
 public:
     explicit ClientConnectedEvent(int client_id);

     int getClientId();
 private:
     int mClientId = -1;
};

#endif /* ClientConnectedEvent_hpp */
