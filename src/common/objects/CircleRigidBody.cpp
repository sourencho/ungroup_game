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
void CircleRigidBody::applyImpulse(const Impulse& impulse) {
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

    setTargetVelocity(velocity);
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

void CircleRigidBody::hermiteInterpolation(sf::Vector2f position, sf::Vector2f velocity, float a,
                                           sf::Int32 delta_ms) {
    float t = a;
    float t2 = t * t;
    float t3 = t2 * t;
    m_position = (2.f * t3 - 3.f * t2 + 1.f) * m_position +
                 (t3 - 2.f * t2 + t) * delta_ms * m_velocity + (-2.f * t3 + 3.f * t2) * position +
                 (t3 - t2) * delta_ms * velocity;
    m_velocity =
        1.f / delta_ms *
        ((6.f * t2 - 6.f * t) * m_position + (3.f * t2 - 4.f * t + 1.f) * delta_ms * m_velocity +
         (-6.f * t2 + 6.f * t) * position + (3.f * t2 - 2.f * t) * delta_ms * velocity);
}
