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
    void reliable_client_recv(sf::TcpSocket* reliable_client) {}
    void reliable_client_send(sf::TcpSocket* reliable_client) {}


    sf::TcpSocket* create_reliable_client() {
        sf::TcpSocket* reliable_client = new sf::TcpSocket;
        reliable_client->connect(SERVER_IP, 4844);
        return reliable_client;
    }

    sf::UdpSocket* create_unreliable_client() {
        sf::UdpSocket* unreliable_client = new sf::UdpSocket;
        unreliable_client->bind(0);
        return unreliable_client;
    }

 public:
     NetworkingClient();
     ~NetworkingClient();

     GameState getGameState();
     int getPlayerId();

     void setClientUnreliableUpdate(ClientUnreliableUpdate client_unreliable_update);
     void setClientReliableUpdate(ClientReliableUpdate client_reliable_update);

 private:
     // Methods
     void readRegistrationResponse();
     void registerNetworkingClient();
     void unreliableClientSend();
     void unreliableClientRecv();
     void reliableClientSend();
     void reliableClientRecv();
     void syncServerState();

     void sendClientUnreliableUpdate();
     void sendClientReliableUpdate();
     void sendPlayerIdRequest();

     // Variables
     sf::Uint32 mClientId;
     sf::Uint32 mCurrentTick;
     bool mIsRegistered = false;

     sf::TcpSocket* mReliableClient;
     sf::UdpSocket* mUnreliableClient;

     ThreadSafeData<GameState> mGameState;
     ThreadSafeData<ClientUnreliableUpdate> mClientUnreliableUpdate;
     ThreadSafeData<ClientReliableUpdate> mClientReliableUpdate;
     ThreadSafeData<int> mPlayerId;
};

#endif /* NetworkingClient_hpp */
