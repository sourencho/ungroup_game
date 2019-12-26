#include "CircleGameObject.hpp"

#include "../util/game_settings.hpp"

sf::Clock shader_clock;

CircleGameObject::CircleGameObject(uint32_t id, sf::Vector2f position, float radius,
                                   PhysicsController& pc, float mass, bool movable) :
    GameObject(id),
    m_circleRigidBody(pc.add(std::unique_ptr<CircleRigidBody>(
        new CircleRigidBody(id, radius, position, mass, movable)))) {
    shader_clock.restart();
}

void CircleGameObject::setActive(bool is_active) {
    m_isActive = is_active;
    m_circleRigidBody.setActive(is_active);
}

void CircleGameObject::applyInput(sf::Vector2f input) {
    m_circleRigidBody.applyInput(input);
}

sf::Vector2f CircleGameObject::getCenter() const {
    sf::Vector2f position = m_circleRigidBody.getPosition();
    float radius = m_circleRigidBody.getRadius();
    return sf::Vector2f(position.x + radius, position.y + radius);
}

sf::Vector2f CircleGameObject::getVelocity() const {
    return m_circleRigidBody.getVelocity();
}