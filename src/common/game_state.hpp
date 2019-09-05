#ifndef game_state_hpp
#define game_state_hpp

#include <stdio.h>
#include <vector>

#include <SFML/Network.hpp>

#include "../common/Group.hpp"
#include "../common/Mine.hpp"


struct GameState {
    sf::Uint32 tick;
    std::vector<GroupUpdate> group_updates;
    std::vector<MineUpdate> mine_updates;
};

sf::Packet pack_game_state(GameState game_state);
GameState unpack_game_state(sf::Packet game_state_packet);

struct ClientUDPUpdate {
    sf::Vector2f direction = sf::Vector2f(0.f, 0.f);
};

struct ClientTCPUpdate {
    bool groupable = false;
};

struct ClientIdAndUDPUpdate {
    int client_id;
    ClientUDPUpdate client_udp_update;
};

struct ClientIdAndTCPUpdate {
    int client_id;
    ClientTCPUpdate client_tcp_update;
};

struct ClientInputs {
    std::vector<int> new_client_ids;
    std::vector<int> removed_client_ids;
    std::vector<ClientIdAndUDPUpdate> client_id_and_udp_updates;
    std::vector<ClientIdAndTCPUpdate> client_id_and_tcp_updates;
};

sf::Packet& operator <<(sf::Packet& packet, const ClientTCPUpdate& client_tcp_update);
sf::Packet& operator >>(sf::Packet& packet, ClientTCPUpdate& client_tcp_update);

sf::Packet& operator <<(sf::Packet& packet, const ClientUDPUpdate& client_udp_update);
sf::Packet& operator >>(sf::Packet& packet, ClientUDPUpdate& client_udp_update);

#endif /* game_state_hpp */
