#include "CollisionDetector.hpp"
#include <iostream>
#include "util.hpp"

CollisionDetector::CollisionDetector() {
    //
}

void CollisionDetector::detectGroupCollisions(const std::vector<Group*> groups) {
    for(Group* group_a: groups) {
        for(Group* group_b: groups) {
            if (group_a == group_b) {
                continue;
            }
            
            // detect collision
            float dist = distance(group_a->getPosition(), group_b->getPosition());
            if (dist < group_a->getRadius() + group_b->getRadius()) {
                std::cout << "Collision" << std::endl;
            } else {
                std::cout << "Nothing" << std::endl;
            }
        }
    }
}
