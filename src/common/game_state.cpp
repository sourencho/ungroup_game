#include "game_state.hpp"
#include "network_util.hpp"


sf::Packet pack_game_state(GameState game_state) {
    sf::Packet packet;
    packet << game_state.tick;

    packet << static_cast<sf::Uint32>(game_state.group_updates.size());
    for (const auto group_update : game_state.group_updates) {
        packet << group_update;
    }

    packet << static_cast<sf::Uint32>(game_state.mine_updates.size());
    for (const auto mine_update : game_state.mine_updates) {
        packet << mine_update;
    }

    return packet;
}

GameState unpack_game_state(sf::Packet game_state_packet) {
    sf::Uint32 tick;
    sf::Uint32 group_updates_size;
    std::vector<GroupUpdate> group_updates;
    sf::Uint32 mine_updates_size;
    std::vector<MineUpdate> mine_updates;

    game_state_packet >> tick;

    game_state_packet >> group_updates_size;
    for (int i=0; i < group_updates_size; ++i) {
        GroupUpdate gu = {};
        game_state_packet >> gu;
        group_updates.push_back(gu);
    }

    game_state_packet >> mine_updates_size;
    for (int i=0; i < mine_updates_size; ++i) {
        MineUpdate mu = {};
        game_state_packet >> mu;
        mine_updates.push_back(mu);
    }

    GameState game_state = {tick, group_updates, mine_updates};
    return game_state;
};
