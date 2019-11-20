#ifndef NetworkingClient_hpp
#define NetworkingClient_hpp

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
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
    std::pair<bool, uint32_t> getPlayerId() const;
    int getClientId() const;
    bool getGameStateIsFresh() const;
    void pushClientUnreliableUpdate(ClientUnreliableUpdate client_unreliable_update);
    void pushClientReliableUpdate(ClientReliableUpdate client_reliable_update);

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
    void addEventListeners();
    void handlePlayerCreatedEvent(std::shared_ptr<Event> event);

    // Misc
    std::atomic<bool> m_playerIdAvialable_ta{false};
    std::atomic<uint32_t> m_playerId_ta{0};

    std::atomic<int> m_clientId_ta{-1};
    std::atomic<uint> m_tick_ta{0};
    std::atomic<bool> m_gameStateIsFresh_ta{true};

    std::mutex m_gameState_lock;
    GameState m_gameState_t;

    std::mutex m_clientUnreliableUpdates_lock;
    std::queue<ClientUnreliableUpdate> m_clientUnreliableUpdates_t;

    std::mutex m_clientReliableUpdates_lock;
    std::queue<ClientReliableUpdate> m_clientReliableUpdates_t;

    sf::Uint16 m_serverUdpPort = 0;
};

#endif /* NetworkingClient_hpp */
