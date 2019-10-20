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

    std::mutex mTcpSocket_lock;
    std::unique_ptr<sf::TcpSocket> mTcpSocket_t;

    std::mutex mUdpSocket_lock;
    std::unique_ptr<sf::UdpSocket> mUdpSocket_t;

    // Threads
    void unreliableSend();
    void unreliableRecv();
    void reliableSend();
    void reliableRecv();

    std::thread mReliableRecv;
    std::thread mReliableSend;
    std::thread mUnreliableRecv;
    std::thread mUnreliableSend;

    std::atomic<bool> mStopThreads_ta{false};

    // Methods
    void sendClientUnreliableUpdate();
    void sendClientReliableUpdate();
    void sendPlayerIdRequest();
    bool readRegistrationResponse();
    bool registerNetworkingClient();

    // Misc
    std::atomic<int> mPlayerId_ta{-1};
    std::atomic<int> mClientId_ta{-1};
    std::atomic<uint> mTick_ta{0};
    std::atomic<bool> mGameStateIsFresh_ta{true};

    std::mutex mGameState_lock;
    GameState mGameState_t;

    std::mutex mClientUnreliableUpdate_lock;
    ClientUnreliableUpdate mClientUnreliableUpdate_t;

    std::mutex mClientReliableUpdate_lock;
    ClientReliableUpdate mClientReliableUpdate_t;

    sf::Uint16 mServerUdpPort = 0;
};

#endif /* NetworkingClient_hpp */
