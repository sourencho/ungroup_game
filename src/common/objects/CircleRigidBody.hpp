#ifndef CircleRigidBody_hpp
#define CircleRigidBody_hpp

#include <memory>
#include <vector>

#include "GameObject.hpp"
#include <SFML/Graphics.hpp>

class CircleRigidBody : public GameObject {
  public:
    CircleRigidBody(uint32_t id, float radius, sf::Vector2f position, bool movable = true);

    void move(sf::Vector2f offset);
    void step(sf::Int32 delta_ms);

    sf::Vector2f getCenter() const;

    float getRadius() const { return mRadius; }
    void setRadius(float radius) { mRadius = radius; }

    sf::Vector2f getPosition() const { return mPosition; }
    void setPosition(sf::Vector2f position) { mPosition = position; }

    void setVelocity(sf::Vector2f velocity) { mVelocity = velocity; }
    void setAcceleration(sf::Vector2f acceleration) { mAcceleration = acceleration; }

    void applyForce(sf::Vector2f force) { mForce += force; }
    void applyInput(sf::Vector2f input);

    const bool isMovable() const { return mMovable; };

  private:
    bool mMovable;
    float mRadius;
    float mMass;
    sf::Vector2f mAcceleration;
    sf::Vector2f mForce;
    sf::Vector2f mPosition; // top left corner of box surrounding circle
    sf::Vector2f mVelocity; // distance/second
};

#endif /* CircleRigidBody_hpp */
