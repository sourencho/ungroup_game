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
    void deleteClient(sf::TcpSocket* client, std::list<sf::TcpSocket*> clients);
    void registerClient(sf::TcpSocket& client);
    void sendPlayerId(sf::TcpSocket& client);
    void sendState(sf::UdpSocket& rt_server, sf::IpAddress& sender, unsigned short port);
    void handleReliableCommand(sf::Socket::Status status, sf::Packet command_packet,
      sf::SocketSelector& selector, sf::TcpSocket& client, std::list<sf::TcpSocket*>& clients);
    void handleUnreliableCommand(sf::Socket::Status status, sf::Packet command_packet,
      sf::UdpSocket& rt_server, sf::IpAddress& sender, unsigned short port);
    void setClientReliableUpdate(sf::Packet packet, int client_id);
    void setClientUnreliableUpdate(sf::Packet packet, int client_id, unsigned int client_tick);

    std::vector<int> getClientIds();

    ThreadSafeMap<sf::TcpSocket*, sf::Int32> mClientSocketsToIds;
    ThreadSafeMap<int, int> mClientToPlayerIds;
    ThreadSafeVector<ClientIdAndUnreliableUpdate> mClientIdAndUnreliableUpdates;
    ThreadSafeVector<ClientIdAndReliableUpdate> mClientIdAndReliableUpdates;

    ThreadSafeData<GameState> mGameState;

    sf::Uint32 mClientIdCounter = 0;
    std::atomic<uint> mTick;
    std::list<sf::TcpSocket*> mClients;
};

#endif /* NetworkingServer_hpp */
