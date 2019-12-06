#include "StateDef.hpp"

#include "network_util.hpp"

sf::Packet& operator<<(sf::Packet& packet, const GameStateObject& game_state_object) {
    packet << static_cast<sf::Uint32>(game_state_object.group_updates.size());
    for (const auto group_update : game_state_object.group_updates) {
        packet << group_update;
    }

    packet << static_cast<sf::Uint32>(game_state_object.mine_updates.size());
    for (const auto mine_update : game_state_object.mine_updates) {
        packet << mine_update;
    }

    packet << static_cast<sf::Uint32>(game_state_object.player_updates.size());
    for (const auto player_update : game_state_object.player_updates) {
        packet << player_update;
    }

    packet << game_state_object.gcu;
    packet << game_state_object.rcu;

    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, GameStateObject& game_state_object) {
    sf::Uint32 group_updates_size;
    std::vector<GroupUpdate> group_updates;
    sf::Uint32 mine_updates_size;
    std::vector<MineUpdate> mine_updates;
    sf::Uint32 player_updates_size;
    std::vector<PlayerUpdate> player_updates;
    GroupControllerUpdate gcu;
    ResourceControllerUpdate rcu;

    packet >> group_updates_size;
    for (int i = 0; i < group_updates_size; ++i) {
        GroupUpdate gu = {};
        packet >> gu;
        group_updates.push_back(gu);
    }

    packet >> mine_updates_size;
    for (int i = 0; i < mine_updates_size; ++i) {
        MineUpdate mu = {};
        packet >> mu;
        mine_updates.push_back(mu);
    }

    packet >> player_updates_size;
    for (int i = 0; i < player_updates_size; ++i) {
        PlayerUpdate pu = {};
        packet >> pu;
        player_updates.push_back(pu);
    }

    packet >> gcu;
    packet >> rcu;

    game_state_object = {group_updates, mine_updates, player_updates, gcu, rcu};

    return packet;
}

sf::Packet& operator<<(sf::Packet& packet, const GameStateCore& game_state_core) {
    return packet << game_state_core.tick << game_state_core.status
                  << game_state_core.winner_player_id;
}

sf::Packet& operator>>(sf::Packet& packet, GameStateCore& game_state_core) {
    packet >> game_state_core.tick >> game_state_core.status >> game_state_core.winner_player_id;
    return packet;
}

sf::Packet& operator<<(sf::Packet& packet, const GameState& game_state) {
    return packet << game_state.object << game_state.core;
}
sf::Packet& operator>>(sf::Packet& packet, GameState& game_state) {
    return packet >> game_state.object >> game_state.core;
}

sf::Packet& operator<<(sf::Packet& packet, const GameStatus& game_status) {
    return packet << static_cast<sf::Uint32>(game_status);
}

sf::Packet& operator>>(sf::Packet& packet, GameStatus& game_status) {
    sf::Uint32 status_index;
    packet >> status_index;
    game_status = GameStatus(status_index);
    return packet;
}