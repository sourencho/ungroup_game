#ifndef util_hpp
#define util_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>

sf::Vector2f normalize(const sf::Vector2f& source);
float distance(const sf::Vector2f& point_a, const sf::Vector2f& point_b);

#endif /* util_hpp */
