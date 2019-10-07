#ifndef NetworkingClient_hpp
#define NetworkingClient_hpp

#include <stdio.h>
#include <vector>
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>

#include <SFML/Network.hpp>

#include "../../common/util/game_def.hpp"
#include "../../common/util/game_state.hpp"


class NetworkingClient {
    const char* SERVER_IP = "127.0.0.1";

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
    // Methods
    bool readRegistrationResponse();
    bool registerNetworkingClient();
    void unreliableClientSend();
    void unreliableClientRecv();
    void reliableClientSend();
    void reliableClientRecv();
    void syncServerState();

    void sendClientUnreliableUpdate();
    void sendClientReliableUpdate();
    void sendPlayerIdRequest();

    void createTcpSocket(unsigned short port);
    void createUdpSocket();

    // Variables

    std::mutex mTcpSocket_lock;
    std::mutex mUdpSocket_lock;
    std::unique_ptr<sf::TcpSocket> mTcpSocket_t;
    std::unique_ptr<sf::UdpSocket> mUdpSocket_t;

    std::atomic<int> mPlayerId_ta{-1};
    std::atomic<int> mClientId_ta{-1};
    std::atomic<uint> mTick_ta{0};
    std::atomic<bool> mGameStateIsFresh_ta{true};
    std::atomic<bool> mStopThreads_ta{false};

    std::mutex mGameState_lock;
    GameState mGameState_t;

    std::mutex mClientUnreliableUpdate_lock;
    ClientUnreliableUpdate mClientUnreliableUpdate_t;

    std::mutex mClientReliableUpdate_lock;
    ClientReliableUpdate mClientReliableUpdate_t;

    std::thread mReliableClientRecv;
    std::thread mReliableClientSend;
    std::thread mUnreliableClientRecv;
    std::thread mUnreliableClientSend;
    std::thread mSyncServerState;
};

#endif /* NetworkingClient_hpp */
