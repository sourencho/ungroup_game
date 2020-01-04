#ifndef game_def_hpp
#define game_def_hpp

#include <SFML/Network.hpp>

struct UnreliableCommand {
    sf::Uint32 client_id;
    sf::Uint32 command;
    sf::Uint32 tick;
};

struct ReliableCommand {
    sf::Uint32 client_id;
    sf::Uint32 command;
    sf::Uint32 tick;
};

enum ReliableCommandType { register_client, player_id, reliable_input };
enum UnreliableCommandType { unreliable_input, nat_punch };

enum GameObjectType {
    player,
    group,
    mine,
};

#endif /* game_def_hpp */
