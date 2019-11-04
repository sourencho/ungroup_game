#ifndef CollisionUtil_hpp
#define CollisionUtil_hpp

#include "../objects/CircleRigidBody.hpp"
#include "PhysicsDef.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <stdio.h>

/**
 * Details of collision between two rigid bodies.
 * @ids: ids of the two rigid bodies.
 * @position: point of contact between the rigid bodies after collision is resolved.
 * @normal: normal of the collision from the latter body to the former.
 * @resolution: the amount by which the bodies should be displaced to resolve the collision.
 */
struct Collision {
    std::pair<uint32_t, uint32_t> ids;
    sf::Vector2f position;
    sf::Vector2f normal;
    std::pair<sf::Vector2f, sf::Vector2f> resolution;
    bool collided;
};

namespace CollisionUtil {
    bool areIntersecting(const CircleRigidBody& circle_a, const CircleRigidBody& circle_b);
    Collision getCollision(const CircleRigidBody& circle_a, const CircleRigidBody& circle_b);

    std::pair<PhysicsDef::Impulse, PhysicsDef::Impulse> getImpulses(const CircleRigidBody& circle_a,
                                                                    const CircleRigidBody& circle_b,
                                                                    const Collision& collision);
} // namespace CollisionUtil

#endif /* CollisionUtil_hpp */