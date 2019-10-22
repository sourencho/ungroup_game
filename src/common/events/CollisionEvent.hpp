#ifndef CollisionEvent_hpp
#define CollisionEvent_hpp

#include "Event.hpp"
#include <SFML/Graphics.hpp>

class CollisionEvent : public Event {
  public:
    explicit CollisionEvent(sf::Vector2f position)
        : Event(EVENT_TYPE_COLLISION), mPosition(position) {}

    sf::Vector2f getPosition() { return mPosition; };

  private:
    sf::Vector2f mPosition;
};

#endif /* CollisionEvent_hpp */
