#include "collision.hpp"
#include <iostream>

/**
    Handles collision between circles by pairs.
    To handle the collision between circle A and B, circle B is moved in the direction 
    of the vector spanning from the center of cirlce A to the center of circle B by the 
    amount needed such that the circles are touching but not intersecting.
*/
void handle_circle_collision(std::vector<std::shared_ptr<Circle>>& circles) {
    for(std::shared_ptr<Circle> circle_a: circles) {
        for(std::shared_ptr<Circle> circle_b: circles) {
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
            } else {
                // Do nothing if there is no collision
            }
        }
    }
}
