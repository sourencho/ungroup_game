#ifndef game_def_hpp
#define game_def_hpp

#include <SFML/Graphics.hpp>

#include "Player.hpp"


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

struct PlayerId {
    sf::Uint32 player_id;
};

struct direction {
    float x_dir;
    float y_dir;
};

struct ClientInputs {
    std::vector<int> new_client_ids;
    std::vector<int> removed_client_ids;
    std::vector<PlayerUpdate> player_updates;
};

enum APICommandType {register_client, toggle_groupable, player_id};
enum RealtimeCommandType {client_update, fetch_state};

#endif /* game_def_hpp */
