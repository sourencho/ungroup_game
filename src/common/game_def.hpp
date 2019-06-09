#ifndef game_def_hpp
#define game_def_hpp

#include <SFML/Graphics.hpp>

const int MAX_PLAYER_COUNT = 10;

struct keys {
    sf::Keyboard::Key up, down, left, right;
};

struct group_circle_update {
    sf::Uint32 client_id;
    float x_pos;
    float y_pos;
    float size;
};

struct client_direction_update {
    sf::Uint32 client_id;
    float x_dir;
    float y_dir;
};

enum APICommand { register_client, group, ungroup };
enum RealtimeCommand { move, fetch_state };

#endif /* game_def_hpp */
