#ifndef StateDef_hpp
#define StateDef_hpp

#include <stdio.h>
#include <vector>

#include <SFML/Network.hpp>

#include "../objects/Group.hpp"
#include "../objects/Mine.hpp"
#include "../systems/GroupController.hpp"
#include "../systems/ResourceController.hpp"

struct GameState {
    unsigned int tick;
    std::vector<GroupUpdate> group_updates;
    std::vector<MineUpdate> mine_updates;
    std::vector<PlayerUpdate> player_updates;
    GroupControllerUpdate gcu;
    ResourceControllerUpdate rcu;
};

sf::Packet pack_game_state(GameState game_state);
GameState unpack_game_state(sf::Packet game_state_packet);

#endif /* StateDef_hpp */
