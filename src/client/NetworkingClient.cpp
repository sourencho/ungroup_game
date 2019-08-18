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
#include "../common/game_state.hpp"


NetworkingClient::NetworkingClient():mDirection(0.f, 0.f) {
    mApiClient = create_api_client();
    mRealtimeClient = create_realtime_client();
    RegisterNetworkingClient();   // Sets mClientId, mCurrentTick, mIsRegistered

    std::cout << "Starting ungroup demo client." << std::endl;

    // api
    std::thread ApiClientRecv_thread(&NetworkingClient::ApiClientRecv, this);
    std::thread ApiClientSend_thread(&NetworkingClient::ApiClientSend, this);

    // realtime
    std::thread RealtimeClientRecv_thread(&NetworkingClient::RealtimeClientRecv, this);
    std::thread RealtimeClientSend_thread(&NetworkingClient::RealtimeClientSend, this);

    // syncs authoritative sever state to client at a regular interval
    std::thread SyncServerState_thread(&NetworkingClient::SyncServerState, this);

    ApiClientRecv_thread.detach();
    ApiClientSend_thread.detach();
    RealtimeClientRecv_thread.detach();
    RealtimeClientSend_thread.detach();
    SyncServerState_thread.detach();
}

NetworkingClient::~NetworkingClient() {}

std::vector<GroupUpdate> NetworkingClient::getGroupUpdates() {
    return mGroupUpdates.copy();
}

std::vector<MineUpdate> NetworkingClient::getMineUpdates() {
    return mMineUpdates.copy();
}

void NetworkingClient::setDirection(sf::Vector2f direction) {
    mDirection.set(direction);
}

void NetworkingClient::setGroupable(bool groupable) {
    mNeedsGroupableStateSync = mGroupable != groupable;
    mGroupable = groupable;
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

void NetworkingClient::ApiClientRecv() {
    while (true) {
        sf::Packet api_response;
        ApiCommand api_command;

        if (mApiClient->receive(api_response) == sf::Socket::Done) {
            if (api_response >> api_command &&
                api_command.command == (sf::Uint32)APICommandType::toggle_groupable) {
                // this is a noop, just a place we can add code for when the server acknowledges
                // it received our groupability toggle
            }
        }
    }
}

void NetworkingClient::ApiClientSend() {
    while (true) {
        if (mNeedsGroupableStateSync) {
            sf::Packet group_request;
            if (group_request << (sf::Uint32)APICommandType::toggle_groupable &&
                group_request << (sf::Uint32)mClientId) {
                mApiClient->send(group_request);
            }
            mNeedsGroupableStateSync = false;
        }
    }
}

void NetworkingClient::RealtimeClientRecv() {
    while (true) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        mRealtimeClient->receive(packet, sender, port);
        GameState game_state = unpack_game_state(packet);

        mCurrentTick = game_state.tick;

        mGroupUpdates.clear();
        for (const auto group_update : game_state.group_updates) {
            mGroupUpdates.add(group_update);
        }

        mMineUpdates.clear();
        for (const auto mine_update : game_state.mine_updates) {
            mMineUpdates.add(mine_update);
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
