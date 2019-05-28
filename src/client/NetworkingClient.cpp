#include <SFML/Network.hpp>

#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include "NetworkingClient.hpp"


NetworkingClient::NetworkingClient() {
    mIsRegistered = false;
    mAcceptingPositionRead = true;
    mAcceptingDirectionRead = true;
}

NetworkingClient::~NetworkingClient() {
}

sf::Uint32 NetworkingClient::Start() {
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
    return mClientId;
}

std::vector<position> NetworkingClient::getPositions() {
    while (!mAcceptingPositionRead) {
        //
    }
    return mPositions;
}

void NetworkingClient::setDirection(direction dir) {
    mAcceptingDirectionRead = false;
    mDirection = dir;
    mAcceptingDirectionRead = true;
}

void NetworkingClient::ReadRegistrationResponse() {
    // read registration data
    sf::Packet registration_response;
    sf::Uint32 api_command;
    sf::Uint32 client_id;
    sf::Uint32 tick;

    if (mApiClient->receive(registration_response) == sf::Socket::Done) {
        if (registration_response >> api_command >> client_id >> tick && (api_command == (sf::Uint32)APICommand::register_client)) {
            std::cout << "Registered with ID and current tick: " << client_id << " " << tick << std::endl;
            // TODO: have this function return these values instead of setting them
            mClientId = client_id;
            mCurrentTick = tick;
            mIsRegistered = true;
        }
    }
}

void NetworkingClient::RegisterNetworkingClient()
{
    sf::Packet registration_request;
    if(registration_request << (sf::Uint32)APICommand::register_client) {
        mApiClient->send(registration_request);
        ReadRegistrationResponse();
    }

    if (!mIsRegistered) {
        throw std::runtime_error("Failed to register. Exiting.");
    }

    // TODO: my_client_id should be local and passed to the threads on creation
}

void NetworkingClient::RealtimeClientRecv() {
    sf::Uint32 server_tick;
    sf::Uint32 client_id;
    float x_pos;
    float y_pos;

    while (true) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        mRealtimeClient->receive(packet, sender, port);
        std::cout << "recv: " << sender << " " << port << std::endl;
        // fetch state updates for now
        if (packet >> server_tick) {
            mAcceptingPositionRead = false;
            mPositions.clear();
            while (packet >> client_id >> x_pos >> y_pos) {
                std::cout << "Client: " << client_id << " has position x,y: " << x_pos << " " << y_pos << std::endl;
                position p = {client_id, x_pos, y_pos};
                mPositions.push_back(p);
            }
            // Im not sure what kind of synchronization needs to happen here.
            // If this tick is the most up-to-date we've ever seen, maybe we set the game to it?
            std::cout << "Client positions from server for tick: " << server_tick << std::endl;
            mCurrentTick = server_tick;
            mAcceptingPositionRead = true;
        } else {
            std::cout << "Failed to read server tick from new packet" << std::endl;
        }
    }
}

void NetworkingClient::RealtimeClientSend() {
    while (true) {
        if (mAcceptingDirectionRead) {
            sf::Packet packet;
            sf::Uint32 move_cmd = (sf::Uint32)RealtimeCommand::move;
            if (packet << mClientId << move_cmd << mCurrentTick << mDirection.x_dir << mDirection.y_dir) {
                mRealtimeClient->send(packet, SERVER_IP, 4888);
            } else {
                std::cout << "Failed to form packet" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

// this probably should just be in RealtimeClientSend and RealtimeClientSend
// should poll client state to see if the current tick has a move AND if the move has
// been sent to the server yet. If both are true, then it should try to communicate the move.
// This would decouple local move setting commands from the network communication. They'd be happening
// in different threads, which is a Good Thing(tm) to avoid blocking in the client on network IO.
void NetworkingClient::SyncServerState() {
    while (true) {
        sf::Packet packet;
        sf::Uint32 fetch_state_cmd = (sf::Uint32)RealtimeCommand::fetch_state;
        if (packet << mClientId << fetch_state_cmd << mCurrentTick) {
            mRealtimeClient->send(packet, SERVER_IP, 4888);
        } else {
            std::cout << "Failed to form packet" << std::endl;
        }
        // fetch state constantly
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
