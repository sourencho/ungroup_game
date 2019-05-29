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


class NetworkingServer {

    const unsigned int CMD_DRIFT_THRESHOLD = 5;

    public:
        NetworkingServer();
        ~NetworkingServer();

        void Start();
    private:
        // Methods
        void RealtimeServer();
        void ApiServer();
        void ComputeGameState();
        void DeleteClient(sf::TcpSocket* client, sf::SocketSelector selector);

        // Variables
        std::unordered_map<sf::TcpSocket*, sf::Int32> mClientSocketsToIds;
        std::unordered_map<sf::Uint32, float*> mClientMoves;
        std::unordered_map<sf::Uint32, float*> mClientPositions;
        sf::Uint32 mClientIdCounter;
        std::atomic<uint> mCurrTick;
        std::atomic<bool> mAcceptingMoveCommands;
};

#endif /* NetworkingServer_hpp */
