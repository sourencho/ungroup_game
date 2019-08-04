#include <vector>
#include <memory>
#include <iostream>

#include "PhysicsController.hpp"

#include "../common/util.hpp"

PhysicsController::PhysicsController() {}

std::shared_ptr<CircleRigidBody> PhysicsController::createCRB() {
    std::shared_ptr<CircleRigidBody> crb = std::shared_ptr<CircleRigidBody>(
        new CircleRigidBody(0.f, sf::Vector2f(0.f, 0.f)));
    mCircleRigidBodies.push_back(crb);
    return crb;
}

void PhysicsController::step() {
    for (auto crb : mCircleRigidBodies) {
        crb->step();
    }
}

/**
    Handles collision between CircleRigidBodies by pairs.
    To handle the collision between circle A and B, circle B is moved in the direction
    of the vector spanning from the center of cirlce A to the center of circle B by the
    amount needed such that the circles are touching but not intersecting.
*/
void PhysicsController::handleCollision() {
    std::vector<std::shared_ptr<CircleRigidBody>> active_circles;
    std::copy_if(
        mCircleRigidBodies.begin(), mCircleRigidBodies.end(), std::back_inserter(active_circles),
        [](std::shared_ptr<CircleRigidBody> circle){return circle->isActive();});

    for (std::shared_ptr<CircleRigidBody> circle_a : active_circles) {
        for (std::shared_ptr<CircleRigidBody> circle_b : active_circles) {
            if (circle_a == circle_b) {
                continue;
            }
            // Detect collision
            float dist = distance(circle_a->getPosition(), circle_b->getPosition());
            if (dist < circle_a->getRadius() + circle_b->getRadius()) {
                // Handle collision
                sf::Vector2f between = getVector(circle_a->getPosition(), circle_b->getPosition());
                float radius_sum = circle_a->getRadius() + circle_b->getRadius();
                float distance_between = length(between);

                sf::Vector2f between_norm = normalize(between);
                between_norm *= radius_sum - distance_between;
                circle_b->move(between_norm);
            }
        }
    }
}
