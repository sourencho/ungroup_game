#include "NetworkingClient.hpp"

#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <vector>

#include <SFML/Network.hpp>

#include "../common/network_util.hpp"


NetworkingClient::NetworkingClient() {
    mPlayerId.set(-1);

    mApiClient = create_api_client();
    mRealtimeClient = create_realtime_client();
    registerNetworkingClient();   // Sets mClientId, mCurrentTick, mIsRegistered

    std::cout << "Starting ungroup demo client." << std::endl;

    // api
    std::thread ApiClientRecv_thread(&NetworkingClient::apiClientRecv, this);
    std::thread ApiClientSend_thread(&NetworkingClient::apiClientSend, this);

    // realtime
    std::thread RealtimeClientRecv_thread(&NetworkingClient::realtimeClientRecv, this);
    std::thread RealtimeClientSend_thread(&NetworkingClient::realtimeClientSend, this);

    // syncs authoritative sever state to client at a regular interval
    std::thread SyncServerState_thread(&NetworkingClient::syncServerState, this);

    ApiClientRecv_thread.detach();
    ApiClientSend_thread.detach();
    RealtimeClientRecv_thread.detach();
    RealtimeClientSend_thread.detach();
    SyncServerState_thread.detach();
}

NetworkingClient::~NetworkingClient() {}

GameState NetworkingClient::getGameState() {
    return mGameState.get();
}

int NetworkingClient::getPlayerId() {
    return mPlayerId.get();
}

void NetworkingClient::setClientUDPUpdate(ClientUDPUpdate client_udp_update) {
    mClientUDPUpdate.set(client_udp_update);
}

void NetworkingClient::setClientTCPUpdate(ClientTCPUpdate client_tcp_update) {
    mClientTCPUpdate.set(client_tcp_update);
}

void NetworkingClient::readRegistrationResponse() {
    // read registration data
    sf::Packet registration_response;
    ApiCommand api_command;

    if (mApiClient->receive(registration_response) == sf::Socket::Done) {
        if (registration_response >> api_command &&
            api_command.command == (sf::Uint32)APICommandType::register_client) {
            std::cout
                << "Registered with ID and current tick: "
                << api_command.client_id
                << " "
                << api_command.tick
                << std::endl;
            mClientId = api_command.client_id;
            mCurrentTick = api_command.tick;
            mIsRegistered = true;
        }
    }
}

void NetworkingClient::registerNetworkingClient() {
    sf::Packet registration_request;
    if (registration_request << (sf::Uint32)APICommandType::register_client) {
        mApiClient->send(registration_request);
        readRegistrationResponse();
    }

    if (!mIsRegistered) {
        throw std::runtime_error("Failed to register. Exiting.");
    }
}

void NetworkingClient::apiClientRecv() {
    while (true) {
        sf::Packet api_response;
        ApiCommand api_command;
        if (mApiClient->receive(api_response) == sf::Socket::Done) {
            api_response >> api_command;
            if (api_command.command == (sf::Uint32) APICommandType::player_id) {
                PlayerId pi;
                api_response >> pi;
                mPlayerId.set(static_cast<int>(pi.player_id));
            }
        }
    }
}


void NetworkingClient::apiClientSend() {
    while (true) {
        sendPlayerIdRequest();
        sendClientTCPUpdate();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void NetworkingClient::sendClientTCPUpdate() {
    sf::Packet packet;
    packet << (sf::Uint32)APICommandType::client_tcp_update;
    packet << mClientTCPUpdate.get();
    mApiClient->send(packet);
}

void NetworkingClient::sendPlayerIdRequest() {
    if (mPlayerId.get() == -1) {
        sf::Packet packet;
        packet << (sf::Uint32)APICommandType::player_id;
        mApiClient->send(packet);
    }
}

void NetworkingClient::realtimeClientRecv() {
    while (true) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        mRealtimeClient->receive(packet, sender, port);
        GameState game_state = unpack_game_state(packet);

        mGameState.set(game_state);
        mCurrentTick = game_state.tick;
    }
}

void NetworkingClient::realtimeClientSend() {
    while (true) {
        sendClientUDPUpdate();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void NetworkingClient::sendClientUDPUpdate() {
    sf::Packet packet;
    sf::Uint32 client_udp_update_cmd = (sf::Uint32) RealtimeCommandType::client_udp_update;
    RealtimeCommand realtime_command = {
        mClientId,
        client_udp_update_cmd,
        mCurrentTick,
    };
    packet << realtime_command;
    packet << mClientUDPUpdate.get();
    mRealtimeClient->send(packet, SERVER_IP, 4888);
}

// this probably should just be in realtimeClientSend and realtimeClientSend
// should poll client state to see if the current tick has a move AND if the move has
// been sent to the server yet. If both are true, then it should try to communicate the move.
// This would decouple local move setting commands from the network communication. They'd be
// happening in different threads, which is a Good Thing(tm) to avoid blocking in the client on
// network IO.
void NetworkingClient::syncServerState() {
    while (true) {
        sf::Packet packet;
        sf::Uint32 fetch_state_cmd = (sf::Uint32)RealtimeCommandType::fetch_state;
        RealtimeCommand realtime_command = {mClientId, fetch_state_cmd, mCurrentTick};
        if (packet << realtime_command) {
            mRealtimeClient->send(packet, SERVER_IP, 4888);
        } else {
            std::cout << "Failed to form packet" << std::endl;
        }
        // fetch state constantly
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
