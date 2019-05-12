#include "util.hpp"
#include <cmath>

sf::Vector2f normalize(const sf::Vector2f& source)
{
    float length = sqrt(
        (source.x * source.x) +
        (source.y * source.y)
    );
    
    if (length != 0)
        return sf::Vector2f(source.x / length, source.y / length);
    else
        return source;
}

float distance(const sf::Vector2f& point_a, const sf::Vector2f& point_b) {
    return sqrt(
        pow((point_b.x - point_a.x), 2) +
        pow((point_b.y - point_a.y), 2)
    );
}
