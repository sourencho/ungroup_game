#include "Event.hpp"

class GroupCollisionEvent : public Event {
 public:
     GroupCollisionEvent(int group_1_id, int group_2_id);

     int getGroup1Id();
     int getGroup2Id();
 private:
     int mGroup1Id = -1;
     int mGroup2Id = -1;
};
