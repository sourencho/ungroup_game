#include <SFML/Network.hpp>
#include "NetworkingClient.hpp"
#include "../common/network_util.hpp"
#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <vector>


NetworkingClient::NetworkingClient():mDirection(0.f, 0.f) {
    mApiClient = create_api_client();
    mRealtimeClient = create_realtime_client();
    RegisterNetworkingClient();   // Sets mClientId, mCurrentTick, mIsRegistered

    std::cout << "Starting ungroup demo client." << std::endl;

    /*
    // api
    std::thread api_client_recv_thread(api_client_recv, &api_client);
    std::thread api_client_send_thread(api_client_send, &api_client);
    */

    // realtime
    std::thread RealtimeClientRecv_thread(&NetworkingClient::RealtimeClientRecv, this);
    std::thread RealtimeClientSend_thread(&NetworkingClient::RealtimeClientSend, this);

    // syncs authoritative sever state to client at a regular interval
    std::thread SyncServerState_thread(&NetworkingClient::SyncServerState, this);

    RealtimeClientRecv_thread.detach();
    RealtimeClientSend_thread.detach();
    SyncServerState_thread.detach();
}

NetworkingClient::~NetworkingClient() {}

std::vector<ClientGroupUpdate> NetworkingClient::getClientGroupUpdates() {
    return mClientGroupUpdates.copy();
}

void NetworkingClient::setDirection(sf::Vector2f direction) {
    mDirection.set(direction);
}

void NetworkingClient::ReadRegistrationResponse() {
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

void NetworkingClient::RegisterNetworkingClient() {
    sf::Packet registration_request;
    if (registration_request << (sf::Uint32)APICommandType::register_client) {
        mApiClient->send(registration_request);
        ReadRegistrationResponse();
    }

    if (!mIsRegistered) {
        throw std::runtime_error("Failed to register. Exiting.");
    }
}

void NetworkingClient::RealtimeClientRecv() {
    sf::Uint32 server_tick;
    while (true) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        mRealtimeClient->receive(packet, sender, port);
        // fetch state updates for now
        if (packet >> server_tick) {
            mClientGroupUpdates.clear();
            ClientGroupUpdate client_group_update;
            while (packet >> client_group_update) {
                mClientGroupUpdates.push_back(client_group_update);
            }
            // Im not sure what kind of synchronization needs to happen here.
            // If this tick is the most up-to-date we've ever seen, maybe we set the game to it?
            mCurrentTick = server_tick;
        } else {
            std::cout << "Failed to read server tick from new packet" << std::endl;
        }
    }
}

void NetworkingClient::RealtimeClientSend() {
    while (true) {
        sf::Packet packet;
        sf::Uint32 move_cmd = (sf::Uint32)RealtimeCommandType::move;
        sf::Vector2f direction = mDirection.copy();
        RealtimeCommand realtime_command = {mClientId, move_cmd, mCurrentTick};
        if (packet << realtime_command << direction) {
            mRealtimeClient->send(packet, SERVER_IP, 4888);
        } else {
            std::cout << "Failed to form packet" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// this probably should just be in RealtimeClientSend and RealtimeClientSend
// should poll client state to see if the current tick has a move AND if the move has
// been sent to the server yet. If both are true, then it should try to communicate the move.
// This would decouple local move setting commands from the network communication. They'd be
// happening in different threads, which is a Good Thing(tm) to avoid blocking in the client on
// network IO.
void NetworkingClient::SyncServerState() {
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
