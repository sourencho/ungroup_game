#ifndef NetworkingClient_hpp
#define NetworkingClient_hpp

#include <stdio.h>
#include <vector>
#include <atomic>

#include <SFML/Network.hpp>

#include "../common/game_def.hpp"
#include "../common/ThreadSafeVector.hpp"
#include "../common/ThreadSafeData.hpp"
#include "../common/ThreadSafeVector2f.hpp"
#include "../common/game_state.hpp"


class NetworkingClient {
    const char* SERVER_IP = "127.0.0.1";

    // these will be strictly for ungroup/group/register/deregister and other Big abstraction
    // changes not sure it's really necessary, but implementing retry logic for something like
    // ungrouping specifically would be annoying. We're a lot more OK with duplication/out of order
    // sends/dropped datagrams for stuff like moving.
    void api_client_recv(sf::TcpSocket* api_client) {}
    void api_client_send(sf::TcpSocket* api_client) {}


    sf::TcpSocket* create_api_client() {
        sf::TcpSocket* api_client = new sf::TcpSocket;
        api_client->connect(SERVER_IP, 4844);
        return api_client;
    }

    sf::UdpSocket* create_realtime_client() {
        sf::UdpSocket* realtime_client = new sf::UdpSocket;
        realtime_client->bind(0);
        return realtime_client;
    }

 public:
     NetworkingClient();
     ~NetworkingClient();

     GameState getGameState();
     int getPlayerId();

     void setClientUDPUpdate(ClientUDPUpdate client_udp_update);
     void setClientTCPUpdate(ClientTCPUpdate client_tcp_update);

 private:
     // Methods
     void readRegistrationResponse();
     void registerNetworkingClient();
     void realtimeClientSend();
     void realtimeClientRecv();
     void apiClientSend();
     void apiClientRecv();
     void syncServerState();

     void sendClientUDPUpdate();
     void sendClientTCPUpdate();
     void sendPlayerIdRequest();

     // Variables
     sf::Uint32 mClientId;
     sf::Uint32 mCurrentTick;
     bool mIsRegistered = false;

     sf::TcpSocket* mApiClient;
     sf::UdpSocket* mRealtimeClient;

     ThreadSafeData<GameState> mGameState;
     ThreadSafeData<ClientUDPUpdate> mClientUDPUpdate;
     ThreadSafeData<ClientTCPUpdate> mClientTCPUpdate;
     ThreadSafeData<int> mPlayerId;
};

#endif /* NetworkingClient_hpp */
