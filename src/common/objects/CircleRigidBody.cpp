#include <algorithm>
#include <cmath>

#include "../physics/CollisionUtil.hpp"
#include "../physics/VectorUtil.hpp"
#include "CircleRigidBody.hpp"

const float TRANSITION_SPEED = 10.f; // drag / friction
const float PLAYER_VELOCITY = 200.f;

CircleRigidBody::CircleRigidBody(uint32_t id, float radius, sf::Vector2f position, float mass,
                                 bool movable)
    : GameObject(id), m_radius(radius), m_position(position), m_velocity(0.f, 0.f), m_mass(mass),
      m_targetVelocity(0.f, 0.f), m_movable(movable) {}

void CircleRigidBody::applyInput(sf::Vector2f input) { m_targetVelocity = input * PLAYER_VELOCITY; }

/**
 * Apply impulse to change velocity based on mass.
 * Formulas taken from http://www.chrishecker.com/Rigid_Body_Dynamics
 */
void CircleRigidBody::applyImpulse(const PhysicsDef::Impulse& impulse) {
    if (!isActive() || !isMovable()) {
        return;
    }

    sf::Vector2f velocity = m_velocity + (impulse.magnitude / m_mass) * impulse.normal;
    setVelocity(velocity);
}

void CircleRigidBody::setVelocity(sf::Vector2f velocity) {
    if (!isActive() || !isMovable()) {
        return;
    }

    m_targetVelocity = velocity;
    m_velocity = velocity;
}

void CircleRigidBody::move(sf::Vector2f offset) {
    if (!isActive() || !isMovable()) {
        return;
    }

    m_position += offset;
}

void CircleRigidBody::step(sf::Int32 delta_ms) {
    if (!isActive() || !isMovable()) {
        return;
    }

    float delta = static_cast<float>(delta_ms) / 1000.f;

    // Update velocity and position via Semi-implicit Euler
    // Instead of acceleration lerp towards a target velocity.
    m_velocity = VectorUtil::lerp(m_velocity, m_targetVelocity, delta * TRANSITION_SPEED);
    m_position += m_velocity * delta;
}

sf::Vector2f CircleRigidBody::getCenter() const {
    sf::Vector2f position = getPosition();
    float radius = getRadius();
    return sf::Vector2f(position.x + radius, position.y + radius);
}