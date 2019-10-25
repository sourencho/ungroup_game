#ifndef CollisionEvent_hpp
#define CollisionEvent_hpp

#include "../physics/PhysicsController.hpp"
#include "Event.hpp"
#include <SFML/Graphics.hpp>

class CollisionEvent : public Event {
  public:
    explicit CollisionEvent(Collision collision)
        : Event(EVENT_TYPE_COLLISION), mCollision(collision) {}

    const Collision getCollision() const { return mCollision; };

  private:
    Collision mCollision;
};

#endif /* CollisionEvent_hpp */
