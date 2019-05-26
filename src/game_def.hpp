#ifndef game_def_hpp
#define game_def_hpp

#include <SFML/Graphics.hpp>

const int MAX_PLAYER_COUNT = 10;

struct keys {
    sf::Keyboard::Key up, down, left, right;
};

struct position {
    sf::Uint32 id;
    sf::Uint32 x_pos;
    sf::Uint32 y_pos;
};

#endif /* game_def_hpp */
