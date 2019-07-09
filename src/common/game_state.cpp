#include "game_state.hpp"
#include "network_util.hpp"


sf::Packet pack_game_state(GameState game_state) {
    sf::Packet packet;
    packet << game_state.tick;
    packet << static_cast<sf::Uint32>(game_state.client_group_updates.size());
    for (const auto client_group_update : game_state.client_group_updates) {
        packet << client_group_update;
    }
    return packet;
}

GameState unpack_game_state(sf::Packet game_state_packet) {
    sf::Uint32 client_group_updates_size;
    sf::Uint32 tick;
    std::vector<ClientGroupUpdate> client_group_updates;

    game_state_packet >> tick;
    game_state_packet >> client_group_updates_size;

    for (int i=0; i < client_group_updates_size; ++i) {
        ClientGroupUpdate cgu;
        game_state_packet >> cgu;
        client_group_updates.push_back(cgu);
    }

    GameState game_state = {tick, client_group_updates};
    return game_state;
};
