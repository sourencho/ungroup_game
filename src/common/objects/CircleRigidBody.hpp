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

    void setMass(float mass) {
        m_mass = mass;
    }
    float getMass() const {
        return m_mass;
    }

    float getRadius() const {
        return m_radius;
    }
    void setRadius(float radius) {
        m_radius = radius;
    }

    sf::Vector2f getPosition() const {
        return m_position;
    }
    void setPosition(sf::Vector2f position) {
        m_position = position;
    }

    sf::Vector2f getVelocity() const {
        return m_velocity;
    }
    void setVelocity(sf::Vector2f velocity);

    void applyImpulse(const PhysicsDef::Impulse& impulse);
    void applyInput(sf::Vector2f input);

    const bool isMovable() const {
        return m_movable;
    };

  private:
    bool m_movable;                // if false then position can't change
    float m_radius;                // m
    float m_mass;                  // kg
    sf::Vector2f m_position;       // top left corner of box surrounding circle
    sf::Vector2f m_velocity;       // distance/second
    sf::Vector2f m_targetVelocity; // velocity will eventually converge to this value
};

#endif /* CircleRigidBody_hpp */
