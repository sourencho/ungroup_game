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
#include "../../common/util/game_state.hpp"

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

    std::mutex m_UdpSocket_lock;
    std::unique_ptr<sf::UdpSocket> m_UdpSocket_t;

    // Threads
    void reliableRecvSend();
    void unreliableRecv();
    void broadcastGameState();

    std::thread m_ReliableRecvSend;
    std::thread m_UnreliableRecv;
    std::thread m_BroadcastGameStateThread;

    std::atomic<bool> m_StopThreads_ta{false};

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
    std::vector<std::pair<sf::Uint32, std::unique_ptr<sf::TcpSocket>>> m_Clients;

    std::mutex m_ClientToPlayerIds_lock;
    std::unordered_map<int, sf::Uint32> m_ClientToPlayerIds_t;

    std::mutex m_ClientToUdpPorts_lock;
    std::unordered_map<int, sf::Uint16> m_ClientToUdpPorts_t;

    std::mutex m_ClientReliableUpdates_lock;
    std::vector<ClientIdAndReliableUpdate> m_ClientReliableUpdates_t;

    std::mutex m_ClientUnreliableUpdates_lock;
    std::vector<ClientIdAndUnreliableUpdate> m_ClientUnreliableUpdates_t;

    std::mutex m_GameState_lock;
    GameState m_GameState_t;

    sf::Uint32 m_ClientIdCounter = 0;

    std::atomic<uint> m_Tick_ta{0};
};

#endif /* NetworkingServer_hpp */
