#ifndef game_def_hpp
#define game_def_hpp

#include <SFML/Graphics.hpp>

const int MAX_PLAYER_COUNT = 10;

struct keys {
    sf::Keyboard::Key up, down, left, right;
};

struct position {
    sf::Uint32 id;
    float x_pos;
    float y_pos;
};

struct direction {
    float x_dir;
    float y_dir;
};

#endif /* game_def_hpp */
