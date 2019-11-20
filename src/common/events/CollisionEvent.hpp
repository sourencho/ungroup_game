#ifndef CollisionEvent_hpp
#define CollisionEvent_hpp

#include "../physics/PhysicsController.hpp"
#include "Event.hpp"
#include <SFML/Graphics.hpp>

class CollisionEvent : public Event {
  public:
    explicit CollisionEvent(Collision collision) :
        Event(EVENT_TYPE_COLLISION), m_collision(collision) {
    }

    const Collision getCollision() const {
        return m_collision;
    };

  private:
    Collision m_collision;
};

#endif /* CollisionEvent_hpp */
