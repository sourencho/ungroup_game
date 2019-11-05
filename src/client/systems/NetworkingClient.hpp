#ifndef NetworkingClient_hpp
#define NetworkingClient_hpp

#include <atomic>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <vector>

#include <SFML/Network.hpp>

#include "../../common/util/game_def.hpp"
#include "../../common/util/game_state.hpp"

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

    std::mutex m_TcpSocket_lock;
    std::unique_ptr<sf::TcpSocket> m_TcpSocket_t;

    std::mutex m_UdpSocket_lock;
    std::unique_ptr<sf::UdpSocket> m_UdpSocket_t;

    // Threads
    void unreliableSend();
    void unreliableRecv();
    void reliableSend();
    void reliableRecv();

    std::thread m_ReliableRecv;
    std::thread m_ReliableSend;
    std::thread m_UnreliableRecv;
    std::thread m_UnreliableSend;

    std::atomic<bool> m_StopThreads_ta{false};

    // Methods
    void sendClientUnreliableUpdate();
    void sendClientReliableUpdate();
    void sendPlayerIdRequest();
    bool readRegistrationResponse();
    bool registerNetworkingClient();

    // Misc
    std::atomic<int> m_PlayerId_ta{-1};
    std::atomic<int> m_ClientId_ta{-1};
    std::atomic<uint> m_Tick_ta{0};
    std::atomic<bool> m_GameStateIsFresh_ta{true};

    std::mutex m_GameState_lock;
    GameState m_GameState_t;

    std::mutex m_ClientUnreliableUpdate_lock;
    ClientUnreliableUpdate m_ClientUnreliableUpdate_t;

    std::mutex m_ClientReliableUpdate_lock;
    ClientReliableUpdate m_ClientReliableUpdate_t;

    sf::Uint16 m_ServerUdpPort = 0;
};

#endif /* NetworkingClient_hpp */
