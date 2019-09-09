#include "GroupCollisionEvent.hpp"

GroupCollisionEvent::GroupCollisionEvent(int group_1_id, int group_2_id)
  :Event(EventType::EVENT_TYPE_GROUP_COLLISION) {
    mGroup1Id = group_1_id;
    mGroup2Id = group_2_id;
}

int GroupCollisionEvent::getGroup1Id() {
    return mGroup1Id;
}

int GroupCollisionEvent::getGroup2Id() {
    return mGroup1Id;
}
