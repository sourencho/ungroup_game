#ifndef CircleGameObject_hpp
#define CircleGameObject_hpp

#include <iostream>
#include <memory>
#include <vector>

#include "../physics/PhysicsController.hpp"
#include "CircleRigidBody.hpp"
#include "GameObject.hpp"

class CircleGameObject : public GameObject {
  public:
    CircleGameObject(uint32_t id, sf::Vector2f position, float radius, PhysicsController& pc,
                     float mass, bool movable = true);

    void applyInput(sf::Vector2f input);

    void setActive(bool is_active);

    void setMass(float mass) {
        m_circleRigidBody.setMass(mass);
    }

    sf::Vector2f getCenter() const;

    float getRadius() const {
        return m_circleRigidBody.getRadius();
    };
    void setRadius(float radius) {
        m_circleRigidBody.setRadius(radius);
    };

    sf::Vector2f getPosition() const {
        return m_circleRigidBody.getPosition();
    };
    void setPosition(sf::Vector2f position) {
        m_circleRigidBody.setPosition(position);
    };

    void linearInterpolatePosition(const sf::Vector2f position, float a) {
        m_circleRigidBody.linearInterpolatePosition(position, a);
    }

    void interpolateVelocity(sf::Vector2f velocity, float a) {
        m_circleRigidBody.interpolateVelocity(velocity, a);
    }

    void hermiteInterpolatePosition(sf::Vector2f position, sf::Vector2f velocity, float a,
                                    sf::Int32 delta_ms) {
        m_circleRigidBody.hermiteInterpolatePosition(position, velocity, a, delta_ms);
    }

    void setCenterPosition(sf::Vector2f position) {
        setPosition({position.x - getRadius(), position.y - getRadius()});
    }

    sf::Vector2f getVelocity() const;

    const CircleRigidBody& getRigidBody() const {
        return m_circleRigidBody;
    }

  protected:
    CircleRigidBody& m_circleRigidBody;
};

#endif /* CircleGameObject_hpp */
