#ifndef NetworkingServer_hpp
#define NetworkingServer_hpp

#include <stdio.h>
#include <iostream>
#include <list>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <utility>
#include <unordered_map>

#include <SFML/Network.hpp>

#include "../../common/util/ThreadSafeMap.hpp"
#include "../../common/util/ThreadSafeVector.hpp"
#include "../../common/util/ThreadSafeData.hpp"
#include "../../common/objects/Group.hpp"
#include "../../common/util/game_state.hpp"
#include "../../common/objects/Mine.hpp"


class NetworkingServer {
    const unsigned int CMD_DRIFT_THRESHOLD = 100;

 public:
    NetworkingServer();
    ~NetworkingServer();

    ClientInputs collectClientInputs();
    void setState(GameState gs);
    void setClientToPlayerId(int client_id, int player_id);
    void incrementTick();
    unsigned int getTick() const;
    void setTick(unsigned int tick);

 private:
    void unreliableServer();

    void reliableServer();
    void clientDisconnect(sf::TcpSocket& client, sf::Uint32 client_id);
    void registerClient(sf::TcpSocket& client, sf::Uint32 client_id);
    void sendPlayerId(sf::TcpSocket& socket, sf::Uint32 client_id);
    void handleReliableCommand(sf::Socket::Status status, sf::Packet command_packet,
        sf::SocketSelector& selector, sf::TcpSocket& socket, sf::Uint32 client_id);
    void handleUnreliableCommand(sf::Socket::Status status, sf::Packet command_packet,
      sf::UdpSocket& rt_server, sf::IpAddress& sender, unsigned short port);
    void setClientReliableUpdate(sf::Packet packet, int client_id);
    void setClientUnreliableUpdate(sf::Packet packet, int client_id, unsigned int client_tick);
    void sendState(sf::UdpSocket& rt_server, sf::IpAddress& sender, unsigned short port);

    std::vector<int> getClientIds();
    std::vector<std::pair<sf::Uint32,std::unique_ptr<sf::TcpSocket>>> mClients;

    ThreadSafeMap<int, int> mClientToPlayerIds;
    ThreadSafeVector<ClientIdAndUnreliableUpdate> mClientIdAndUnreliableUpdates;
    ThreadSafeVector<ClientIdAndReliableUpdate> mClientIdAndReliableUpdates;

    ThreadSafeData<GameState> mGameState;

    sf::Uint32 mClientIdCounter = 0;
    std::atomic<uint> mTick;

    std::thread mReliableThread;
    std::thread mUnreliableThread;

    std::atomic<bool> mStopThreads;
};

#endif /* NetworkingServer_hpp */
