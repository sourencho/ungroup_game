#include "CollisionDetector.hpp"
#include <iostream>

CollisionDetector::CollisionDetector() {
    //
}

void CollisionDetector::handleGroupShapeCollision(GroupShape* group_a, GroupShape* group_b) {
    // Get vector between group centers
    sf::Vector2f between = getVector(group_a->getPosition(), group_b->getPosition());

    float radius_sum = group_a->getRadius() + group_b->getRadius();
    float distance_between = length(between);

    sf::Vector2f between_norm = normalize(between);

    between_norm *= radius_sum - distance_between;

    group_b->move(between_norm);
}

void CollisionDetector::detectAndHandleGroupShapeCollisoin(std::vector<GroupShape*> group_shapes) {
    for(GroupShape* group_a: group_shapes) {
        for(GroupShape* group_b: group_shapes) {
            if (group_a == group_b) {
                continue;
            }

            // detect and handle collision
            float dist = distance(group_a->getPosition(), group_b->getPosition());
            if (dist < group_a->getRadius() + group_b->getRadius()) {
                handleGroupShapeCollision(group_a, group_b);
            } else {
                // Nothing
            }
        }
    }
}
