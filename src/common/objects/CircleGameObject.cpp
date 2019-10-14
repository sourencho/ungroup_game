#include "CircleGameObject.hpp"


CircleGameObject::CircleGameObject(uint32_t id, sf::Vector2f position, float radius,
  sf::Color color, std::shared_ptr<PhysicsController> pc):
  GameObject(id), mCircle(std::shared_ptr<Circle>(new Circle(radius, position, color))),
  mCircleRigidBody(pc->createCRB(id, radius, position)) {}

std::shared_ptr<Circle> CircleGameObject::getCircle() {
    return mCircle;
}

void CircleGameObject::setActive(bool is_active) {
    mIsActive = is_active;
    mCircleRigidBody->setActive(is_active);
}

void CircleGameObject::setVelocity(sf::Vector2f velocity) {
    mCircleRigidBody->setVelocity(velocity);
}

void CircleGameObject::setRadius(float radius) {
    mCircle->setRadius(radius);
    mCircleRigidBody->setRadius(radius);
}

void CircleGameObject::setPosition(sf::Vector2f position) {
    mCircle->setPosition(position);
    mCircleRigidBody->setPosition(position);
}

void CircleGameObject::matchRigid() {
    mCircle->setPosition(mCircleRigidBody->getPosition());
}
