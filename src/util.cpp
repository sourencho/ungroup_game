#include "util.hpp"
#include <cmath>

float length(const sf::Vector2f& source) {
    return sqrt(
        (source.x * source.x) +
        (source.y * source.y)
    );
}

sf::Vector2f normalize(const sf::Vector2f& source) {
    float l = length(source);
    
    if (l != 0)
        return sf::Vector2f(source.x / l, source.y / l);
    else
        return source;
}

float distance(const sf::Vector2f& point_a, const sf::Vector2f& point_b) {
    return sqrt(
        pow((point_b.x - point_a.x), 2) +
        pow((point_b.y - point_a.y), 2)
    );
}

sf::Vector2f getVector(const sf::Vector2f& point_a, const sf::Vector2f& point_b) {
    return  sf::Vector2f(point_b.x - point_a.x, point_b.y - point_a.y);
}
