#include "CircleGameObject.hpp"


CircleGameObject::CircleGameObject(unsigned int id, sf::Vector2f position, float radius)
    :GameObject(id),
    mCircle(std::shared_ptr<Circle>(new Circle(radius, position))),
    mCircleRigidBody(std::shared_ptr<CircleRigidBody>(new CircleRigidBody(radius, position))) {}

std::shared_ptr<Circle> CircleGameObject::getCircle() {
    return mCircle;
}

std::shared_ptr<CircleRigidBody> CircleGameObject::getCircleRigidBody() {
    return mCircleRigidBody;
}

std::vector<std::shared_ptr<Circle>> CircleGameObject::getCircles(
    std::vector<std::shared_ptr<CircleGameObject>>& circle_game_objects) {
    std::vector<std::shared_ptr<Circle>> circles;
    std::transform(
        circle_game_objects.begin(), circle_game_objects.end(), std::back_inserter(circles),
        [](std::shared_ptr<CircleGameObject> cgo){return cgo->getCircle();});
    return circles;
};

void CircleGameObject::setActive(bool is_active) {
    mIsActive = is_active;
    mCircleRigidBody->setActive(is_active);
};

void CircleGameObject::setVelocity(sf::Vector2f velocity) {
    mCircleRigidBody->setVelocity(velocity);
}

void CircleGameObject::setRadius(float radius) {
    mCircle->setRadius(radius);
    mCircleRigidBody->setRadius(radius);
}

void CircleGameObject::matchRigid() {
    mCircle->setPosition(mCircleRigidBody->getPosition());
}
