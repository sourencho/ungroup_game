#include "Event.hpp"


class ClientConnectedEvent : public Event {
 public:
     explicit ClientConnectedEvent(int client_id);

     int getClientId();
 private:
     int mClientId = -1;
};
