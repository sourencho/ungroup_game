#ifndef NetworkingServer_hpp
#define NetworkingServer_hpp

#include <stdio.h>
#include <SFML/Network.hpp>
#include <iostream>
#include <list>
#include <thread>
#include <future>
#include <chrono>
#include <unordered_map>
#include "../common/game_def.hpp"
#include "Group.hpp"


class NetworkingServer {

    const unsigned int CMD_DRIFT_THRESHOLD = 5;

    public:
        NetworkingServer();
        ~NetworkingServer();

        void Start();
        std::vector<network_player> getNetworkPlayers();
        void setState(std::vector<Group*> active_groups);
        std::vector<int> getClientIds();
    private:
        // Methods
        void RealtimeServer();
        void ApiServer();
        void ComputeGameState();

        void DeleteClient(sf::TcpSocket* client, sf::SocketSelector selector);
        void Move(sf::Packet command_packet, sf::Uint32 client_id, sf::Uint32 tick);
        void RegisterClient(sf::TcpSocket& client);

        // Variables
        std::unordered_map<sf::TcpSocket*, sf::Int32> mClientSocketsToIds;
        std::unordered_map<sf::Uint32, float*> mClientMoves;
        std::vector<network_game_object> mNetworkGameObjects;
        sf::Uint32 mClientIdCounter;
        std::atomic<uint> mCurrTick;
        std::atomic<bool> mAcceptingMoveCommands;
        std::atomic<bool> mAcceptingNetworkGameObjectsReads;
        std::atomic<bool> mAcceptingClientSocketToIdsReads;
};

#endif /* NetworkingServer_hpp */
