#ifndef CircleRigidBody_hpp
#define CircleRigidBody_hpp

#include <memory>
#include <vector>

#include "../physics/PhysicsDef.hpp"
#include "../physics/VectorUtil.hpp"
#include "GameObject.hpp"
#include <SFML/Graphics.hpp>

class CircleRigidBody : public GameObject {
  public:
    CircleRigidBody(uint32_t id, float radius, sf::Vector2f position, float mass,
                    bool movable = true);

    void move(sf::Vector2f offset);
    void step(sf::Int32 delta_ms);

    sf::Vector2f getCenter() const;

    void setMass(float mass) { mMass = mass; }
    float getMass() const { return mMass; }

    float getRadius() const { return mRadius; }
    void setRadius(float radius) { mRadius = radius; }

    sf::Vector2f getPosition() const { return mPosition; }
    void setPosition(sf::Vector2f position) { mPosition = position; }

    sf::Vector2f getVelocity() const { return mVelocity; }
    void setVelocity(sf::Vector2f velocity);

    void applyImpulse(const PhysicsDef::Impulse& impulse);
    void applyInput(sf::Vector2f input);

    const bool isMovable() const { return mMovable; };

  private:
    bool mMovable;                // if false then position can't change
    float mRadius;                // m
    float mMass;                  // kg
    sf::Vector2f mPosition;       // top left corner of box surrounding circle
    sf::Vector2f mVelocity;       // distance/second
    sf::Vector2f mTargetVelocity; // velocity will eventually converge to this value
};

#endif /* CircleRigidBody_hpp */
