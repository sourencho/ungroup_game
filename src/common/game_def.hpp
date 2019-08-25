#ifndef game_def_hpp
#define game_def_hpp

#include <SFML/Graphics.hpp>

struct Keys {
    sf::Keyboard::Key up, down, left, right, group;
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
    float radius;
    bool groupable;
};

struct MineUpdate {
    sf::Uint32 mine_id;
    float x_pos;
    float y_pos;
    float radius;
};

struct client_direction_update {
    sf::Uint32 client_id;
    float x_dir;
    float y_dir;
};

struct client_groupability_update{
    sf::Uint32 client_id;
    bool groupable;
};

struct direction {
    float x_dir;
    float y_dir;
};

struct client_inputs {
    std::vector<int> new_client_ids;
    std::vector<int> removed_client_ids;
    std::vector<client_direction_update> client_direction_updates;
    std::vector<client_groupability_update> client_groupability_updates;
};

enum APICommandType { register_client, toggle_groupable};
enum RealtimeCommandType { move, fetch_state };

#endif /* game_def_hpp */
