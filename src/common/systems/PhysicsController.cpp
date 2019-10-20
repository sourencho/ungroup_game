#include <iostream>
#include <memory>
#include <vector>

#include "PhysicsController.hpp"

#include "../util/util.hpp"

PhysicsController::PhysicsController() {}

std::shared_ptr<CircleRigidBody> PhysicsController::createCRB(uint32_t id, float radius,
                                                              sf::Vector2f position) {
    std::shared_ptr<CircleRigidBody> crb =
        std::shared_ptr<CircleRigidBody>(new CircleRigidBody(id, radius, position));
    mCircleRigidBodies.push_back(crb);
    return crb;
}

void PhysicsController::update(sf::Int32 delta_ms) {
    step(delta_ms);
    handleCollision();
}

/**
 * Update positions of all physics bodies.
 */
void PhysicsController::step(sf::Int32 delta_ms) {
    for (auto crb : mCircleRigidBodies) {
        crb->step(delta_ms);
    }
}

/**
 * Handles collision between CircleRigidBodies by pairs.
 * To handle the collision between circle A and B, circle B is moved in the direction
 * of the vector spanning from the center of cirlce A to the center of circle B by the
 * amount needed such that the circles are touching but not intersecting.
 */
void PhysicsController::handleCollision() {
    for (std::shared_ptr<CircleRigidBody> circle_a : mCircleRigidBodies) {
        if (!circle_a->isActive()) {
            continue;
        }
        for (std::shared_ptr<CircleRigidBody> circle_b : mCircleRigidBodies) {
            if (!circle_b->isActive()) {
                continue;
            }
            if (circle_a == circle_b) {
                continue;
            }
            // Detect collision
            float dist = distance(circle_a->getCenter(), circle_b->getCenter());
            if (dist < circle_a->getRadius() + circle_b->getRadius()) {
                // Handle collision
                sf::Vector2f between = getVector(circle_a->getCenter(), circle_b->getCenter());
                float radius_sum = circle_a->getRadius() + circle_b->getRadius();
                float distance_between = length(between);

                sf::Vector2f between_norm = normalize(between);
                between_norm *= radius_sum - distance_between;
                circle_b->move(between_norm);
            }
        }
    }
}
