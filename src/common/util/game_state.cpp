#include "game_state.hpp"

#include "network_util.hpp"


sf::Packet pack_game_state(GameState game_state) {
    sf::Packet packet;

    if (!(packet << game_state.tick && packet << static_cast<sf::Uint32>(
        game_state.group_updates.size()))) {
        std::cout << "Failed to form packet" << std::endl;
    }
    for (const auto group_update : game_state.group_updates) {
        if (!(packet << group_update)) {
            std::cout << "Failed to form packet" << std::endl;
        }
    }

    if (!(packet << static_cast<sf::Uint32>(game_state.mine_updates.size()))) {
        std::cout << "Failed to form packet" << std::endl;
    }
    for (const auto mine_update : game_state.mine_updates) {
        if (!(packet << mine_update)) {
            std::cout << "Failed to form packet" << std::endl;
        }
    }

    if (!(packet << static_cast<sf::Uint32>(game_state.player_updates.size()))) {
        std::cout << "Failed to form packet" << std::endl;
    }
    for (const auto player_update : game_state.player_updates) {
        if (!(packet << player_update)) {
            std::cout << "Failed to form packet" << std::endl;
        }
    }

    return packet;
}

GameState unpack_game_state(sf::Packet game_state_packet) {
    sf::Uint32 tick;
    sf::Uint32 group_updates_size;
    std::vector<GroupUpdate> group_updates;
    sf::Uint32 mine_updates_size;
    std::vector<MineUpdate> mine_updates;
    sf::Uint32 player_updates_size;
    std::vector<PlayerUpdate> player_updates;

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

    game_state_packet >> player_updates_size;
    for (int i=0; i < player_updates_size; ++i) {
        PlayerUpdate pu = {};
        game_state_packet >> pu;
        player_updates.push_back(pu);
    }

    GameState game_state = {tick, group_updates, mine_updates, player_updates};
    return game_state;
}

sf::Packet& operator <<(sf::Packet& packet,
  const ClientUnreliableUpdate& client_unreliable_update) {
    return packet
        << client_unreliable_update.direction;
}

sf::Packet& operator >>(sf::Packet& packet, ClientUnreliableUpdate& client_unreliable_update) {
    return packet
        >> client_unreliable_update.direction;
}

sf::Packet& operator <<(sf::Packet& packet, const ClientReliableUpdate& client_reliable_update) {
    return packet
        << client_reliable_update.groupable;
}

sf::Packet& operator >>(sf::Packet& packet, ClientReliableUpdate& client_reliable_update) {
    return packet
        >> client_reliable_update.groupable;
}
