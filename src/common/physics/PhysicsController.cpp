#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "PhysicsController.hpp"

#include "../events/CollisionEvent.hpp"
#include "../events/EventController.hpp"

PhysicsController::PhysicsController() {}

CircleRigidBody& PhysicsController::add(std::unique_ptr<CircleRigidBody> circle_rigid_body) {
    CircleRigidBody& circle_rigid_body_ref = *circle_rigid_body;
    mCircleRigidBodies.push_back(std::move(circle_rigid_body));
    return circle_rigid_body_ref;
}

void PhysicsController::update(sf::Int32 delta_ms) {
    step(delta_ms);
    resolveCollisions();
}

/**
 * Update positions of all physics bodies.
 */
void PhysicsController::step(sf::Int32 delta_ms) {
    for (auto& crb : mCircleRigidBodies) {
        crb->step(delta_ms);
    }
}

/**
 * Detects and resolves collision between all active CircleRigidBodies by pairs.
 */
void PhysicsController::resolveCollisions() {
    auto inactive_crbs = std::stable_partition(
        mCircleRigidBodies.begin(), mCircleRigidBodies.end(),
        [](const std::unique_ptr<CircleRigidBody>& crb) { return crb->isActive(); });

    for (auto circle_a_it = mCircleRigidBodies.begin(); circle_a_it != inactive_crbs;
         ++circle_a_it) {
        for (auto circle_b_it = mCircleRigidBodies.begin(); circle_b_it != inactive_crbs;
             ++circle_b_it) {
            CircleRigidBody& circle_a = **circle_a_it;
            CircleRigidBody& circle_b = **circle_b_it;

            if (CollisionUtil::areIntersecting(circle_a, circle_b)) {
                Collision collision = CollisionUtil::getCollision(circle_a, circle_b);
                resolveCollision(circle_a, circle_b, collision);
                fireCollisionEvent(collision);
            }
        }
    }
}

void PhysicsController::resolveCollision(CircleRigidBody& circle_a, CircleRigidBody& circle_b,
                                         const Collision& collision) {
    circle_a.move(collision.resolution.first);
    circle_b.move(collision.resolution.second);
}

void PhysicsController::fireCollisionEvent(const Collision& collision) {
    EventController::getInstance().forceQueueEvent(
        std::move(std::unique_ptr<CollisionEvent>(new CollisionEvent(collision))));
}
