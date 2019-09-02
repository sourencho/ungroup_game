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

     ClientInputs collectClientInputs();
     void setState(
        std::vector<std::shared_ptr<Group>> groups,
        std::vector<std::shared_ptr<Mine>> mines);
     void setClientToPlayerId(int client_id, int player_id);
     void incrementTick();

 private:
     void realtimeServer();
     void apiServer();
     void deleteClient(sf::TcpSocket* client, std::list<sf::TcpSocket*> clients);
     void registerClient(sf::TcpSocket& client);
     void sendPlayerId(sf::TcpSocket& client);
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

     std::vector<int> getClientIds();
     std::vector<int> popNewClientIds();
     std::vector<int> popRemovedClientIds();
     std::vector<PlayerUpdate> popPlayerUpdates();

     ThreadSafeMap<sf::TcpSocket*, sf::Int32> mClientSocketsToIds;
     ThreadSafeMap<int, int> mClientToPlayerIds;
     ThreadSafeVector<int> mNewClientIds;
     ThreadSafeVector<int> mRemovedClientIds;
     ThreadSafeVector<PlayerUpdate> mPlayerUpdates;

     ThreadSafeData<GameState> mGameState;

     sf::Uint32 mClientIdCounter = 0;
     std::atomic<uint> mCurrTick;
     std::list<sf::TcpSocket*> mClients;
};

#endif /* NetworkingServer_hpp */
