#include "CircleGameObject.hpp"


CircleGameObject::CircleGameObject(
    unsigned int id, sf::Vector2f position, float radius, std::shared_ptr<CircleRigidBody> crb):
    GameObject(id),
    mCircle(std::shared_ptr<Circle>(new Circle(radius, position))),
    mCircleRigidBody(crb) {
    crb->setPosition(position);
    crb->setRadius(radius);
}

std::shared_ptr<Circle> CircleGameObject::getCircle() {
    return mCircle;
}

std::shared_ptr<CircleRigidBody> CircleGameObject::getCircleRigidBody() {
    return mCircleRigidBody;
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
