#ifndef NetworkingServer_hpp
#define NetworkingServer_hpp

#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <future>
#include <unordered_map>

#include <SFML/Network.hpp>

#include "../common/ThreadSafeMap.hpp"
#include "../common/game_def.hpp"
#include "../common/ThreadSafeVector.hpp"
#include "../common/ThreadSafeData.hpp"
#include "../common/game_state.hpp"
#include "../common/Group.hpp"
#include "../common/Mine.hpp"


class NetworkingServer {
    const unsigned int CMD_DRIFT_THRESHOLD = 5;

 public:
     NetworkingServer();
     ~NetworkingServer();

     client_inputs collectClientInputs();
     void setState(
        std::vector<std::shared_ptr<Group>> groups,
        std::vector<std::shared_ptr<Mine>> mines);
     void incrementTick();

 private:
     void realtimeServer();
     void apiServer();
     void deleteClient(sf::TcpSocket* client, std::list<sf::TcpSocket*> clients);
     void move(sf::Packet command_packet, sf::Uint32 client_id, sf::Uint32 tick);
     void updateGroupable(sf::TcpSocket& client);
     void registerClient(sf::TcpSocket& client);
     void handleApiCommand(
        sf::Socket::Status status,
        sf::Packet command_packet,
        sf::SocketSelector& selector,
        sf::TcpSocket& client,
        std::list<sf::TcpSocket*>& clients);
    void handleRealtimeCommand(
        sf::Socket::Status status,
        sf::Packet command_packet,
        sf::UdpSocket& rt_server,
        sf::IpAddress& sender,
        unsigned short port);

     std::vector<client_direction_update> getClientDirectionUpdates();
     std::vector<client_groupability_update> getClientGroupabilityUpdates();
     std::vector<int> getClientIds();
     std::vector<int> popNewClientIds();
     std::vector<int> popRemovedClientIds();

     ThreadSafeMap<sf::TcpSocket*, sf::Int32> mClientSocketsToIds;
     ThreadSafeVector<int> mNewClientIds;
     ThreadSafeVector<int> mRemovedClientIds;
     ThreadSafeMap<sf::Uint32, sf::Vector2f> mClientMoves;
     ThreadSafeMap<sf::Uint32, bool> mClientGroupable;

     ThreadSafeData<GameState> mGameState;

     sf::Uint32 mClientIdCounter = 0;
     std::atomic<uint> mCurrTick;
     std::list<sf::TcpSocket*> mClients;
};

#endif /* NetworkingServer_hpp */
