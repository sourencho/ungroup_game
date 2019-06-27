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

#include "../common/ThreadSafeMap.hpp"
#include "../common/game_def.hpp"
#include "../common/ThreadSafeVector.hpp"
#include "Group.hpp"
#include <SFML/Network.hpp>


class NetworkingServer {
    const unsigned int CMD_DRIFT_THRESHOLD = 5;

 public:
     NetworkingServer();
     ~NetworkingServer();

     client_inputs collectClientInputs();
     void setState(std::vector<std::shared_ptr<Group>> active_groups);
     void incrementTick();
     std::vector<client_direction_update> getClientDirectionUpdates();
     std::vector<int> getClientIds();

 private:
     void RealtimeServer();
     void ApiServer();
     void DeleteClient(sf::TcpSocket* client, sf::SocketSelector selector);
     void Move(sf::Packet command_packet, sf::Uint32 client_id, sf::Uint32 tick);
     void RegisterClient(sf::TcpSocket& client);
     void HandleApiCommand(sf::SocketSelector selector, sf::TcpSocket& client);

     ThreadSafeMap<sf::TcpSocket*, sf::Int32> mClientSocketsToIds;
     ThreadSafeVector<ClientGroupUpdate> mClientGroupUpdates;
     ThreadSafeMap<sf::Uint32, sf::Vector2f> mClientMoves;

     sf::Uint32 mClientIdCounter = 0;
     std::atomic<uint> mCurrTick;
};

#endif /* NetworkingServer_hpp */
