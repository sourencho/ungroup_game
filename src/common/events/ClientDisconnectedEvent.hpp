#ifndef ClientDisconnectedEvent_hpp
#define ClientDisconnectedEvent_hpp

#include "Event.hpp"

class ClientDisconnectedEvent : public Event {
  public:
    explicit ClientDisconnectedEvent(int client_id);

    int getClientId();

  private:
    int mClientId = -1;
};

#endif /* ClientDisconnectedEvent_hpp */
