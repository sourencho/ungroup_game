#include "CollisionHandler.hpp"
#include <iostream>
#include "../common/util.hpp"

void handleGroupCollision(Group* group_a, Group* group_b) {
    // Get vector between group centers
    sf::Vector2f between = getVector(group_a->getPosition(), group_b->getPosition());

    float radius_sum = group_a->getSize() + group_b->getSize();
    float distance_between = length(between);

    sf::Vector2f between_norm = normalize(between);

    between_norm *= radius_sum - distance_between;

    group_b->move(between_norm);
}
