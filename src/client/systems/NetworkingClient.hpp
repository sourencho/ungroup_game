#ifndef NetworkingClient_hpp
#define NetworkingClient_hpp

#include <atomic>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <vector>

#include <SFML/Network.hpp>

#include "../../common/util/StateDef.hpp"
#include "../../common/util/game_def.hpp"

class NetworkingClient {

  public:
    NetworkingClient();
    ~NetworkingClient();

    GameState getGameState();
    void incrementTick();
    uint getTick() const;
    void setTick(uint tick);
    int getPlayerId() const;
    int getClientId() const;
    bool getGameStateIsFresh() const;
    void setClientUnreliableUpdate(ClientUnreliableUpdate client_unreliable_update);
    void setClientReliableUpdate(ClientReliableUpdate client_reliable_update);

  private:
    // Sockets
    void createTcpSocket(unsigned short port);
    void createUdpSocket();

    std::mutex m_tcpSocket_lock;
    std::unique_ptr<sf::TcpSocket> m_tcpSocket_t;

    std::mutex m_udpSocket_lock;
    std::unique_ptr<sf::UdpSocket> m_udpSocket_t;

    // Threads
    void unreliableSend();
    void unreliableRecv();
    void reliableSend();
    void reliableRecv();

    std::thread m_reliableRecv;
    std::thread m_reliableSend;
    std::thread m_unreliableRecv;
    std::thread m_unreliableSend;

    std::atomic<bool> m_stopThreads_ta{false};

    // Methods
    void sendClientUnreliableUpdate();
    void sendClientReliableUpdate();
    void sendPlayerIdRequest();
    bool readRegistrationResponse();
    bool registerNetworkingClient();

    // Misc
    std::atomic<int> m_playerId_ta{-1};
    std::atomic<int> m_clientId_ta{-1};
    std::atomic<uint> m_tick_ta{0};
    std::atomic<bool> m_gameStateIsFresh_ta{true};

    std::mutex m_gameState_lock;
    GameState m_gameState_t;

    std::mutex m_clientUnreliableUpdate_lock;
    ClientUnreliableUpdate m_clientUnreliableUpdate_t;

    std::mutex m_clientReliableUpdate_lock;
    ClientReliableUpdate m_clientReliableUpdate_t;

    sf::Uint16 m_serverUdpPort = 0;
};

#endif /* NetworkingClient_hpp */
