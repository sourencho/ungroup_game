#include "CircleGameObject.hpp"


CircleGameObject::CircleGameObject(unsigned int id, sf::Vector2f position, float size, sf::Color color)
    : GameObject(id), mCircle(std::shared_ptr<Circle>(new Circle(size, position, color))) {}

std::shared_ptr<Circle> CircleGameObject::getCircle() {
    return mCircle;
}

std::vector<std::shared_ptr<Circle>> CircleGameObject::getCircles(
    std::vector<std::shared_ptr<CircleGameObject>>& circle_game_objects) {
    std::vector<std::shared_ptr<Circle>> circles;
    std::transform(
        circle_game_objects.begin(), circle_game_objects.end(), std::back_inserter(circles),
        [](std::shared_ptr<CircleGameObject> cgo){return cgo->getCircle();});
    return circles;
};
