#ifndef game_def_hpp
#define game_def_hpp

#include <SFML/Graphics.hpp>

struct keys {
    sf::Keyboard::Key up, down, left, right;
};

struct RealtimeCommand {
    sf::Uint32 client_id;
    sf::Uint32 command;
    sf::Uint32 tick;
};


struct ApiCommand {
    sf::Uint32 client_id;
    sf::Uint32 command;
    sf::Uint32 tick;
};

struct GroupUpdate {
    sf::Uint32 group_id;
    float x_pos;
    float y_pos;
    float size;
};

struct MineUpdate {
    sf::Uint32 mine_id;
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

enum APICommandType { register_client, group, ungroup };
enum RealtimeCommandType { move, fetch_state };

#endif /* game_def_hpp */
