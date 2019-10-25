#include "CircleGameObject.hpp"

CircleGameObject::CircleGameObject(uint32_t id, sf::Vector2f position, float radius,
                                   sf::Color color, std::shared_ptr<PhysicsController> pc,
                                   bool movable)
    : GameObject(id), mCircle(std::unique_ptr<Circle>(new Circle(radius, position, color))),
      mCircleRigidBody(pc->add(std::move(
          std::unique_ptr<CircleRigidBody>(new CircleRigidBody(id, radius, position, movable))))){};

Circle& CircleGameObject::getCircle() { return *mCircle; }

void CircleGameObject::setActive(bool is_active) {
    mIsActive = is_active;
    mCircleRigidBody.setActive(is_active);
}

void CircleGameObject::applyForce(sf::Vector2f force) { mCircleRigidBody.applyForce(force); }

void CircleGameObject::setVelocity(sf::Vector2f velocity) {
    mCircleRigidBody.setVelocity(velocity);
}

void CircleGameObject::applyInput(sf::Vector2f input) { mCircleRigidBody.applyInput(input); }

void CircleGameObject::setAcceleration(sf::Vector2f acceleration) {
    mCircleRigidBody.setAcceleration(acceleration);
}

void CircleGameObject::setRadius(float radius) {
    mCircle->setRadius(radius);
    mCircleRigidBody.setRadius(radius);
}

void CircleGameObject::setPosition(sf::Vector2f position) {
    mCircle->setPosition(position);
    mCircleRigidBody.setPosition(position);
}

void CircleGameObject::matchRigid() { mCircle->setPosition(mCircleRigidBody.getPosition()); }
