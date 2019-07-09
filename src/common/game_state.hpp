#ifndef game_state_hpp
#define game_state_hpp

#include <stdio.h>
#include <vector>
#include "game_def.hpp"
#include <SFML/Network.hpp>

struct GameState {
    sf::Uint32 tick;
    std::vector<ClientGroupUpdate> client_group_updates;
};

sf::Packet pack_game_state(GameState game_state);
GameState unpack_game_state(sf::Packet game_state_packet);

#endif /* game_state_hpp */
