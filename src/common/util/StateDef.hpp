#ifndef StateDef_hpp
#define StateDef_hpp

#include <stdio.h>
#include <vector>

#include <SFML/Network.hpp>

#include "../objects/Group.hpp"
#include "../objects/Mine.hpp"
#include "../systems/GroupController.hpp"

struct GameState {
    unsigned int tick;
    std::vector<GroupUpdate> group_updates;
    std::vector<MineUpdate> mine_updates;
    std::vector<PlayerUpdate> player_updates;
    GroupControllerUpdate gcu;
};

sf::Packet pack_game_state(GameState game_state);
GameState unpack_game_state(sf::Packet game_state_packet);

struct ClientUnreliableUpdate {
    sf::Vector2f direction = sf::Vector2f(0.f, 0.f);
};

struct ClientReliableUpdate {
    bool toggle_joinable;
    bool toggle_ungroup;
    void setAll(bool value) {
        toggle_joinable = value;
        toggle_ungroup = value;
    };
    bool allFalse() { return !toggle_joinable && !toggle_ungroup; };
};

struct PlayerUnreliableUpdate {
    uint32_t player_id;
    ClientUnreliableUpdate client_unreliable_update;
};

struct PlayerReliableUpdate {
    uint32_t player_id;
    ClientReliableUpdate client_reliable_update;
};

struct PlayerInputs {
    std::vector<PlayerUnreliableUpdate> player_unreliable_updates;
    std::vector<PlayerReliableUpdate> player_reliable_updates;
};

struct ClientInputAndTick {
    ClientUnreliableUpdate cuu;
    ClientReliableUpdate cru;
    unsigned int tick;
};

sf::Packet& operator<<(sf::Packet& packet, const ClientReliableUpdate& cru);
sf::Packet& operator>>(sf::Packet& packet, ClientReliableUpdate& cru);

sf::Packet& operator<<(sf::Packet& packet, const ClientUnreliableUpdate& cuu);
sf::Packet& operator>>(sf::Packet& packet, ClientUnreliableUpdate& cuu);

#endif /* StateDef_hpp */
