#ifndef util_hpp
#define util_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>

sf::Vector2f normalize(const sf::Vector2f& source);
float distance(const sf::Vector2f& point_a, const sf::Vector2f& point_b);
sf::Vector2f getVector(const sf::Vector2f& point_a, const sf::Vector2f& point_b);
float length(const sf::Vector2f& source);

#endif /* util_hpp */
