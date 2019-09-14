#include "CircleRigidBody.hpp"


CircleRigidBody::CircleRigidBody(float radius, sf::Vector2f position):
  mRadius(radius), mPosition(position), mVelocity(0.f, 0.f) {}


void CircleRigidBody::setActive(bool is_active) {
    mIsActive = is_active;
}

bool CircleRigidBody::isActive() const {
    return mIsActive;
}

float CircleRigidBody::getRadius() const {
    return mRadius;
}

sf::Vector2f CircleRigidBody::getPosition() const {
    return mPosition;
}

void CircleRigidBody::setRadius(float radius) {
    mRadius = radius;
}

void CircleRigidBody::setPosition(sf::Vector2f position) {
    mPosition = position;
}

void CircleRigidBody::setVelocity(sf::Vector2f velocity) {
    mVelocity = velocity;
}

void CircleRigidBody::move(sf::Vector2f offset) {
    mPosition += offset;
}

void CircleRigidBody::step() {
    mPosition += mVelocity;
}

sf::Vector2f CircleRigidBody::getCenter() {
    sf::Vector2f position = getPosition();
    float radius = getRadius();
    return sf::Vector2f(position.x + radius, position.y + radius);
}
