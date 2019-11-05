#ifndef CollisionEvent_hpp
#define CollisionEvent_hpp

#include "../physics/PhysicsController.hpp"
#include "Event.hpp"
#include <SFML/Graphics.hpp>

class CollisionEvent : public Event {
  public:
    explicit CollisionEvent(Collision collision)
        : Event(EVENT_TYPE_COLLISION), m_Collision(collision) {}

    const Collision getCollision() const { return m_Collision; };

  private:
    Collision m_Collision;
};

#endif /* CollisionEvent_hpp */
