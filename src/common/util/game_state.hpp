#ifndef game_state_hpp
#define game_state_hpp

#include <stdio.h>
#include <vector>

#include <SFML/Network.hpp>

#include "../objects/Group.hpp"
#include "../objects/Mine.hpp"


struct GameState {
    sf::Uint32 tick;
    std::vector<GroupUpdate> group_updates;
    std::vector<MineUpdate> mine_updates;
};

sf::Packet pack_game_state(GameState game_state);
GameState unpack_game_state(sf::Packet game_state_packet);

struct ClientUnreliableUpdate {
    sf::Vector2f direction = sf::Vector2f(0.f, 0.f);
};

struct ClientReliableUpdate {
    bool groupable = false;
};

struct ClientIdAndUnreliableUpdate {
    int client_id;
    ClientUnreliableUpdate client_unreliable_update;
};

struct ClientIdAndReliableUpdate {
    int client_id;
    ClientReliableUpdate client_reliable_update;
};

struct ClientInputs {
    std::vector<ClientIdAndUnreliableUpdate> client_id_and_unreliable_updates;
    std::vector<ClientIdAndReliableUpdate> client_id_and_reliable_updates;
};

sf::Packet& operator <<(sf::Packet& packet, const ClientReliableUpdate& client_reliable_update);
sf::Packet& operator >>(sf::Packet& packet, ClientReliableUpdate& client_reliable_update);

sf::Packet& operator <<(sf::Packet& packet, const ClientUnreliableUpdate& client_unreliable_update);
sf::Packet& operator >>(sf::Packet& packet, ClientUnreliableUpdate& client_unreliable_update);

#endif /* game_state_hpp */
