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

        // Thread safe mClientGroupUpdates operations
        std::vector<client_group_update> getClientGroupUpdatesCopy();
        void clearClientGroupUpdates();
        void addToClientGroupUpdates(client_group_update cgu);

        // Thread safe mClientMove operations
        void updateClientMoves(sf::Uint32 client_id, float x_dir, float y_dir);
        void eraseFromClientMoves(sf::Uint32 client_id);

        // Thread safe mClientSocketsToIds opreations
        void eraseFromClientSocketsToIds(sf::TcpSocket* client);
        void writeToClientSocketsToIds(sf::TcpSocket* client, sf::Uint32 id);
        sf::Uint32 readFromClientSocketsToIds(sf::TcpSocket* client);

        std::unordered_map<sf::TcpSocket*, sf::Int32> mClientSocketsToIds;
        std::vector<client_group_update> mClientGroupUpdates;
        std::unordered_map<sf::Uint32, float*> mClientMoves;

        std::mutex mClientInputsWriteLock; // protects mClientMoves and mClientSocketsToIds
        std::mutex mClientGroupUpdatesWriteLock; // protects mClientGroupUpdates

        sf::Uint32 mClientIdCounter = 0;
        std::atomic<uint> mCurrTick;
};

#endif /* NetworkingServer_hpp */
