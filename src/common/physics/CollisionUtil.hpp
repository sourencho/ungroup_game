#ifndef CollisionUtil_hpp
#define CollisionUtil_hpp

#include "../objects/CircleRigidBody.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <stdio.h>

struct Collision {
    std::pair<uint32_t, uint32_t> ids;
    sf::Vector2f position;
    sf::Vector2f direction;
    std::pair<sf::Vector2f, sf::Vector2f> resolution;
};

namespace CollisionUtil {
bool areIntersecting(const CircleRigidBody& circle_a, const CircleRigidBody& circle_b);
Collision getCollision(const CircleRigidBody& circle_a, const CircleRigidBody& circle_b);
} // namespace CollisionUtil

#endif /* CollisionUtil_hpp */
