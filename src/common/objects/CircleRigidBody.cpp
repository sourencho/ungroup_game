#include <algorithm>
#include <cmath>

#include "CircleRigidBody.hpp"

CircleRigidBody::CircleRigidBody(uint32_t id, float radius, sf::Vector2f position, bool movable)
    : GameObject(id), mRadius(radius), mPosition(position), mVelocity(0.f, 0.f), mMass(1.f),
      mForce(0.f, 0.f), mAcceleration(0.f, 0.f), mMovable(movable) {}

void CircleRigidBody::applyInput(sf::Vector2f input) { applyForce(input * 5.f); }

void CircleRigidBody::move(sf::Vector2f offset) {
    if (!isActive() || !isMovable()) {
        return;
    }

    mPosition += offset;
}

void CircleRigidBody::step(sf::Int32 delta_ms) {
    if (!isActive() || !isMovable()) {
        return;
    }

    float delta = static_cast<float>(delta_ms) / 1000.f;
    sf::Vector2f& position = mPosition;
    sf::Vector2f& velocity = mVelocity;
    sf::Vector2f& acceleration = mAcceleration;
    sf::Vector2f& force = mForce;
    float& mass = mMass;

    // Velocity Verlet
    sf::Vector2f position_step =
        position + (velocity * delta) + (acceleration * std::pow(2.f, delta) / 2.f);
    sf::Vector2f acceleration_step = force / mass;
    sf::Vector2f velocity_step = velocity + ((acceleration + acceleration_step) * delta / 2.f);

    mPosition = position_step;
    mAcceleration = acceleration_step;
    mVelocity = velocity_step;
    mForce = {0.f, 0.f};
}

sf::Vector2f CircleRigidBody::getCenter() const {
    sf::Vector2f position = getPosition();
    float radius = getRadius();
    return sf::Vector2f(position.x + radius, position.y + radius);
}
