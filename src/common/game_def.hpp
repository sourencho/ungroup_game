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

struct PlayerId {
    sf::Uint32 player_id;
};

enum APICommandType {register_client, toggle_groupable, player_id, client_tcp_update};
enum RealtimeCommandType {client_udp_update, fetch_state};

#endif /* game_def_hpp */
