#include "CollisionUtil.hpp"

#include <cmath>

#include "VectorUtil.hpp"

bool CollisionUtil::areIntersecting(const CircleRigidBody& circle_a,
                                    const CircleRigidBody& circle_b) {
    if (circle_a.getId() == circle_b.getId()) {
        return false;
    }
    float dist = VectorUtil::distance(circle_a.getCenter(), circle_b.getCenter());
    return dist < circle_a.getRadius() + circle_b.getRadius();
}

Collision CollisionUtil::getCollision(const CircleRigidBody& circle_a,
                                      const CircleRigidBody& circle_b) {
    bool collided = true;
    sf::Vector2f between = VectorUtil::getVector(circle_b.getCenter(), circle_a.getCenter());
    float overlap = circle_a.getRadius() + circle_b.getRadius() - VectorUtil::length(between);
    sf::Vector2f normal = VectorUtil::normalize(between);

    sf::Vector2f relative_velocity = circle_a.getVelocity() - circle_b.getVelocity();
    float velocity_along_normal = VectorUtil::dot(relative_velocity, normal);

    // Do not consider collision if velocities are seperating
    if (velocity_along_normal > 0) {
        collided = false;
    }

    std::pair<sf::Vector2f, sf::Vector2f> resolution;
    if (circle_a.isMovable() && circle_b.isMovable()) {
        resolution = std::make_pair(normal * overlap / 2.f, -normal * overlap / 2.f);
    } else if (circle_a.isMovable() && !circle_b.isMovable()) {
        resolution = std::make_pair(normal * overlap, sf::Vector2f(0.f, 0.f));
    } else if (!circle_a.isMovable() && circle_b.isMovable()) {
        resolution = std::make_pair(sf::Vector2f(0.f, 0.f), -normal * overlap / 2.f);
    } else {
        resolution = std::make_pair(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f));
    }

    sf::Vector2f collision_point =
        circle_b.getCenter() + resolution.second + (normal * circle_b.getRadius());

    Collision collision = {
        .ids = std::pair<uint32_t, uint32_t>(circle_a.getId(), circle_b.getId()),
        .position = collision_point,
        .normal = normal,
        .resolution = resolution,
        .collided = collided,
    };

    return collision;
}

std::pair<PhysicsDef::Impulse, PhysicsDef::Impulse>
CollisionUtil::getImpulses(const CircleRigidBody& circle_a, const CircleRigidBody& circle_b,
                           const Collision& collision) {
    sf::Vector2f relative_velocity = circle_a.getVelocity() - circle_b.getVelocity();
    float impulse = -2.f * VectorUtil::dot(relative_velocity, collision.normal) /
                    (1 / circle_a.getMass() + 1 / circle_b.getMass());
    return std::make_pair((PhysicsDef::Impulse){impulse, collision.normal},
                          (PhysicsDef::Impulse){impulse, -collision.normal});
}

bool CollisionUtil::isInBounds(const CircleRigidBody& circle, sf::Vector2f bounds_center,
                               float bounds_radius) {
    return VectorUtil::distance(bounds_center, circle.getCenter()) + circle.getRadius() <=
           bounds_radius;
}

sf::Vector2f CollisionUtil::getBoundsCorrection(const CircleRigidBody& circle,
                                                sf::Vector2f bounds_center, float bounds_radius) {
    sf::Vector2f between = VectorUtil::getVector(circle.getCenter(), bounds_center);
    sf::Vector2f normal = VectorUtil::normalize(between);
    float overlap = circle.getRadius() + bounds_radius - VectorUtil::length(between);
    sf::Vector2f move_circle = normal * (2.f * circle.getRadius() - overlap);
    return move_circle;
}