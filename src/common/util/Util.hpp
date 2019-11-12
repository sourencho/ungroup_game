#ifndef Util_hpp
#define Util_hpp

#include "../physics/VectorUtil.hpp"
#include <SFML/Graphics.hpp>

namespace Util {
    sf::Vector2f inputToDirection(bool toggle_up, bool toggle_down, bool toggle_right,
                                  bool toggle_left, bool toggle_stop) {
        if (toggle_stop) {
            return sf::Vector2f(0.f, 0.f);
        }

        sf::Vector2f direction = sf::Vector2f(0.f, 0.f);
        if (toggle_up) {
            direction += sf::Vector2f(0.f, -1.f);
        }
        if (toggle_down) {
            direction += sf::Vector2f(0.f, 1.f);
        }
        if (toggle_left) {
            direction += sf::Vector2f(-1.f, 0.f);
        }
        if (toggle_right) {
            direction += sf::Vector2f(1.f, 0.f);
        }
        return VectorUtil::normalize(direction);
    };
} // namespace Util

#endif /* VectorUtil_hpp */
