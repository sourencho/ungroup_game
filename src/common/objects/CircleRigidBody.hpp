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

    void setMass(float mass) { m_Mass = mass; }
    float getMass() const { return m_Mass; }

    float getRadius() const { return m_Radius; }
    void setRadius(float radius) { m_Radius = radius; }

    sf::Vector2f getPosition() const { return m_Position; }
    void setPosition(sf::Vector2f position) { m_Position = position; }

    sf::Vector2f getVelocity() const { return m_Velocity; }
    void setVelocity(sf::Vector2f velocity);

    void applyImpulse(const PhysicsDef::Impulse& impulse);
    void applyInput(sf::Vector2f input);

    const bool isMovable() const { return m_Movable; };

  private:
    bool m_Movable;                // if false then position can't change
    float m_Radius;                // m
    float m_Mass;                  // kg
    sf::Vector2f m_Position;       // top left corner of box surrounding circle
    sf::Vector2f m_Velocity;       // distance/second
    sf::Vector2f m_TargetVelocity; // velocity will eventually converge to this value
};

#endif /* CircleRigidBody_hpp */
