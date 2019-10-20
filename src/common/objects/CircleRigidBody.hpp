#ifndef CircleRigidBody_hpp
#define CircleRigidBody_hpp

#include <memory>
#include <vector>

#include "GameObject.hpp"
#include <SFML/Graphics.hpp>

class CircleRigidBody {
  public:
    CircleRigidBody(uint32_t id, float radius, sf::Vector2f position);

    void setActive(bool is_active);
    bool isActive() const;

    float getRadius() const;
    void setRadius(float radius);

    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f position);

    void setVelocity(sf::Vector2f velocity);

    sf::Vector2f getCenter();

    void move(sf::Vector2f offset);
    void step(sf::Int32 delta_ms);

  private:
    bool mIsActive = false;
    float mRadius;
    sf::Vector2f mPosition;
    sf::Vector2f mVelocity; // distance/second
    uint32_t mId;
};

#endif /* CircleRigidBody_hpp */
