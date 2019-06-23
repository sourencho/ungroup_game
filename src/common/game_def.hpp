#ifndef game_def_hpp
#define game_def_hpp

#include <SFML/Graphics.hpp>

const int MAX_PLAYER_COUNT = 10;

struct keys {
    sf::Keyboard::Key up, down, left, right;
};

struct client_group_update {
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

struct direction {
    float x_dir;
    float y_dir;
};

struct client_inputs {
    std::vector<int> client_ids;
    std::vector<client_direction_update> client_direction_updates;
};

enum APICommand { register_client, group, ungroup };
enum RealtimeCommand { move, fetch_state };

#endif /* game_def_hpp */
