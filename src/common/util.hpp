#ifndef util_hpp
#define util_hpp

#include <stdio.h>
#include <sfml/graphics.hpp>

sf::vector2f normalize(const sf::vector2f& source);
float distance(const sf::vector2f& point_a, const sf::vector2f& point_b);
sf::vector2f getvector(const sf::vector2f& point_a, const sf::vector2f& point_b);
float length(const sf::vector2f& source);

#endif /* util_hpp */
