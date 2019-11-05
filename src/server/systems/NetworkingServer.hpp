#ifndef NetworkingServer_hpp
#define NetworkingServer_hpp

#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SFML/Network.hpp>

#include "../../common/objects/Group.hpp"
#include "../../common/objects/Mine.hpp"
#include "../../common/util/StateDef.hpp"

class NetworkingServer {
    const unsigned int CMD_DRIFT_THRESHOLD = 200;

  public:
    NetworkingServer();
    ~NetworkingServer();

    ClientInputs collectClientInputs();
    void setState(const GameState& gs);
    void setClientToPlayerId(int client_id, int player_id);
    void incrementTick();
    unsigned int getTick() const;
    void setTick(unsigned int tick);

  private:
    // Sockets
    void createUdpSocket();

    std::mutex m_udpSocket_lock;
    std::unique_ptr<sf::UdpSocket> m_udpSocket_t;

    // Threads
    void reliableRecvSend();
    void unreliableRecv();
    void broadcastGameState();

    std::thread m_reliableRecvSend;
    std::thread m_unreliableRecv;
    std::thread m_broadcastGameStateThread;

    std::atomic<bool> m_stopThreads_ta{false};

    // Methods
    void clientDisconnect(sf::TcpSocket& client, sf::Uint32 client_id);
    void registerClient(sf::Packet packet, sf::TcpSocket& client, sf::Uint32 client_id);
    void sendPlayerId(sf::TcpSocket& socket, sf::Uint32 client_id);
    void handleReliableCommand(sf::Socket::Status status, sf::Packet command_packet,
                               sf::SocketSelector& selector, sf::TcpSocket& socket,
                               sf::Uint32 client_id);
    void handleUnreliableCommand(sf::Socket::Status status, sf::Packet command_packet,
                                 sf::IpAddress& sender, unsigned short port);
    void setClientReliableUpdate(sf::Packet packet, int client_id);
    void setClientUnreliableUpdate(sf::Packet packet, int client_id, unsigned int client_tick);
    std::vector<int> getClientIds();
    void sendGameState();

    // Misc
    std::vector<std::pair<sf::Uint32, std::unique_ptr<sf::TcpSocket>>> m_clients;

    std::mutex m_clientToPlayerIds_lock;
    std::unordered_map<int, sf::Uint32> m_clientToPlayerIds_t;

    std::mutex m_clientToUdpPorts_lock;
    std::unordered_map<int, sf::Uint16> m_clientToUdpPorts_t;

    std::mutex m_clientReliableUpdates_lock;
    std::vector<ClientIdAndReliableUpdate> m_clientReliableUpdates_t;

    std::mutex m_clientUnreliableUpdates_lock;
    std::vector<ClientIdAndUnreliableUpdate> m_clientUnreliableUpdates_t;

    std::mutex m_gameState_lock;
    GameState m_gameState_t;

    sf::Uint32 m_clientIdCounter = 0;

    std::atomic<uint> m_tick_ta{0};
};

#endif /* NetworkingServer_hpp */
