#include "collision.hpp"
#include <iostream>

void handle_circle_collision(std::vector<Circle*>& circles) {
    for(Circle* circle_a: circles) {
        for(Circle* circle_b: circles) {
            if (circle_a == circle_b) {
                continue;
            }

            // detect and handle collision
            float dist = distance(circle_a->getPosition(), circle_b->getPosition());
            if (dist < circle_a->getRadius() + circle_b->getRadius()) {
                // Get vector between group centers
                sf::Vector2f between = getVector(circle_a->getPosition(), circle_b->getPosition());

                float radius_sum = circle_a->getRadius() + circle_b->getRadius();
                float distance_between = length(between);

                sf::Vector2f between_norm = normalize(between);

                between_norm *= radius_sum - distance_between;

                circle_b->move(between_norm);
            } else {
                // Nothing
            }
        }
    }

}
