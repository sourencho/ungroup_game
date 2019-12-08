#include <algorithm>
#include <cmath>

#include "../physics/CollisionUtil.hpp"
#include "../physics/VectorUtil.hpp"
#include "CircleRigidBody.hpp"

CircleRigidBody::CircleRigidBody(uint32_t id, float radius, sf::Vector2f position, float mass,
                                 bool movable) :
    GameObject(id),
    m_radius(radius), m_position(position), m_velocity(0.f, 0.f), m_mass(mass),
    m_targetVelocity(0.f, 0.f), m_movable(movable) {
}

void CircleRigidBody::applyInput(sf::Vector2f input) {
    setTargetVelocity(input * PhysicsDef::PLAYER_VELOCITY);
}

/**
 * Apply impulse to change velocity based on mass.
 * Formulas taken from http://www.chrishecker.com/Rigid_Body_Dynamics
 */
void CircleRigidBody::applyImpulse(const PhysicsDef::Impulse& impulse) {
    sf::Vector2f velocity = m_velocity + (impulse.magnitude / m_mass) * impulse.normal;
    setVelocity(velocity);
    setTargetVelocity(velocity);
}

void CircleRigidBody::setVelocity(sf::Vector2f velocity) {
    if (!isActive() || !isMovable()) {
        return;
    }

    m_velocity = velocity;
    VectorUtil::clamp(m_velocity, PhysicsDef::MIN_VELOCITY, PhysicsDef::MAX_VELOCITY);
}

void CircleRigidBody::setTargetVelocity(sf::Vector2f target_velocity) {
    if (!isActive() || !isMovable()) {
        return;
    }

    m_targetVelocity = target_velocity;
    VectorUtil::clamp(m_targetVelocity, PhysicsDef::MIN_VELOCITY, PhysicsDef::MAX_VELOCITY);
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
    m_velocity =
        VectorUtil::lerp(m_velocity, m_targetVelocity, delta * PhysicsDef::TRANSITION_SPEED);
    m_position += m_velocity * delta;
}

sf::Vector2f CircleRigidBody::getCenter() const {
    sf::Vector2f position = getPosition();
    float radius = getRadius();
    return sf::Vector2f(position.x + radius, position.y + radius);
}