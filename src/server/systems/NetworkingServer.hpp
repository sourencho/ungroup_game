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

    std::mutex mUdpSocket_lock;
    std::unique_ptr<sf::UdpSocket> mUdpSocket_t;

    // Threads
    void reliableRecvSend();
    void unreliableRecv();
    void broadcastGameState();

    std::thread mReliableRecvSend;
    std::thread mUnreliableRecv;
    std::thread mBroadcastGameStateThread;

    std::atomic<bool> mStopThreads_ta{false};

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
    std::vector<std::pair<sf::Uint32, std::unique_ptr<sf::TcpSocket>>> mClients;

    std::mutex mClientToPlayerIds_lock;
    std::unordered_map<int, sf::Uint32> mClientToPlayerIds_t;

    std::mutex mClientToUdpPorts_lock;
    std::unordered_map<int, sf::Uint16> mClientToUdpPorts_t;

    std::mutex mClientReliableUpdates_lock;
    std::vector<ClientIdAndReliableUpdate> mClientReliableUpdates_t;

    std::mutex mClientUnreliableUpdates_lock;
    std::vector<ClientIdAndUnreliableUpdate> mClientUnreliableUpdates_t;

    std::mutex mGameState_lock;
    GameState mGameState_t;

    sf::Uint32 mClientIdCounter = 0;

    std::atomic<uint> mTick_ta{0};
};

#endif /* NetworkingServer_hpp */
