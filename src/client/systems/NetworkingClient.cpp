#include "NetworkingClient.hpp"

#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <vector>
#include <SFML/Network.hpp>

#include "../../common/util/network_util.hpp"


int UNRELIABLE_UPDATE_SEND_SLEEP = 100;
int RELIABLE_UPDATE_SEND_SLEEP = 100;
int SYNC_SERVER_STATE_SLEEP = 100;


NetworkingClient::NetworkingClient():mTick(0), mGameState() {
    mPlayerId.set(-1);

    mReliableClient = create_reliable_client();
    mUnreliableClient = create_unreliable_client();
    registerNetworkingClient();   // Sets mClientId, mTick, mIsRegistered

    std::cout << "Starting ungroup demo client." << std::endl;

    // reliable
    std::thread ReliableClientRecv_thread(&NetworkingClient::reliableClientRecv, this);
    std::thread ReliableClientSend_thread(&NetworkingClient::reliableClientSend, this);

    // unreliable
    std::thread UnreliableClientRecv_thread(&NetworkingClient::unreliableClientRecv, this);
    std::thread UnreliableClientSend_thread(&NetworkingClient::unreliableClientSend, this);

    // syncs authoritative sever state to client at a regular interval
    std::thread SyncServerState_thread(&NetworkingClient::syncServerState, this);

    ReliableClientRecv_thread.detach();
    ReliableClientSend_thread.detach();
    UnreliableClientRecv_thread.detach();
    UnreliableClientSend_thread.detach();
    SyncServerState_thread.detach();
}

NetworkingClient::~NetworkingClient() {}

GameState NetworkingClient::getGameState() {
    mGameStateIsFresh = false;
    return mGameState.get();
}

int NetworkingClient::getPlayerId() {
    return mPlayerId.get();
}

void NetworkingClient::setClientUnreliableUpdate(ClientUnreliableUpdate client_unreliable_update) {
    mClientUnreliableUpdate.set(client_unreliable_update);
}

void NetworkingClient::setClientReliableUpdate(ClientReliableUpdate client_reliable_update) {
    mClientReliableUpdate.set(client_reliable_update);
}

void NetworkingClient::readRegistrationResponse() {
    // read registration data
    sf::Packet registration_response;
    ReliableCommand reliable_command;

    if (mReliableClient->receive(registration_response) == sf::Socket::Done) {
        if (registration_response >> reliable_command &&
            reliable_command.command == ReliableCommandType::register_client) {
            std::cout
                << "Registered with ID and current tick: "
                << reliable_command.client_id
                << " "
                << reliable_command.tick
                << std::endl;
            mClientId = reliable_command.client_id;

            mTick = reliable_command.tick;

            mIsRegistered = true;
        }
    }
}

void NetworkingClient::registerNetworkingClient() {
    sf::Packet registration_request;
    if (registration_request << ReliableCommandType::register_client) {
        mReliableClient->send(registration_request);
        readRegistrationResponse();
    }

    if (!mIsRegistered) {
        throw std::runtime_error("Failed to register. Exiting.");
    }
}

void NetworkingClient::reliableClientRecv() {
    while (true) {
        sf::Packet reliable_response;
        ReliableCommand reliable_command;
        if (mReliableClient->receive(reliable_response) == sf::Socket::Done) {
            reliable_response >> reliable_command;
            if (reliable_command.command == ReliableCommandType::player_id) {
                PlayerId pi;
                reliable_response >> pi;
                mPlayerId.set(static_cast<int>(pi.player_id));
            }
        }
    }
}

void NetworkingClient::reliableClientSend() {
    while (true) {
        sendPlayerIdRequest();
        // TODO(souren|#59): Don't spam server with TCP calls, optimize when updates are sent
        sendClientReliableUpdate();
        std::this_thread::sleep_for(std::chrono::milliseconds(RELIABLE_UPDATE_SEND_SLEEP));
    }
}

void NetworkingClient::sendClientReliableUpdate() {
    sf::Packet packet;
    if (packet << ReliableCommandType::client_reliable_update &&
        packet << mClientReliableUpdate.get()) {
        mReliableClient->send(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

void NetworkingClient::sendPlayerIdRequest() {
    if (mPlayerId.get() == -1) {
        sf::Packet packet;
        if (packet << (sf::Uint32) ReliableCommandType::player_id) {
            mReliableClient->send(packet);
        } else {
            std::cout << "Failed to form packet" << std::endl;
        }
    }
}

void NetworkingClient::unreliableClientRecv() {
    while (true) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        mUnreliableClient->receive(packet, sender, port);
        GameState game_state = unpack_game_state(packet);

        mGameState.set(game_state);
        //mTick = game_state.tick;
        mGameStateIsFresh = true;
    }
}

void NetworkingClient::unreliableClientSend() {
    while (true) {
        sendClientUnreliableUpdate();
        std::this_thread::sleep_for(std::chrono::milliseconds(UNRELIABLE_UPDATE_SEND_SLEEP));
    }
}

void NetworkingClient::sendClientUnreliableUpdate() {
    sf::Packet packet;
    sf::Uint32 client_unreliable_update_cmd = UnreliableCommandType::client_unreliable_update;
    UnreliableCommand unreliable_command = {
        mClientId,
        client_unreliable_update_cmd,
        mTick,
    };
    if (packet << unreliable_command && packet << mClientUnreliableUpdate.get()) {
        mUnreliableClient->send(packet, SERVER_IP, 4888);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

// this probably should just be in unreliableClientSend and unreliableClientSend
// should poll client state to see if the current tick has a move AND if the move has
// been sent to the server yet. If both are true, then it should try to communicate the move.
// This would decouple local move setting commands from the network communication. They'd be
// happening in different threads, which is a Good Thing(tm) to avoid blocking in the client on
// network IO.
void NetworkingClient::syncServerState() {
    while (true) {
        sf::Packet packet;
        sf::Uint32 fetch_state_cmd = UnreliableCommandType::fetch_state;
        UnreliableCommand unreliable_command = {mClientId, fetch_state_cmd, mTick};
        if (packet << unreliable_command) {
            mUnreliableClient->send(packet, SERVER_IP, 4888);
        } else {
            std::cout << "Failed to form packet" << std::endl;
        }
        // fetch state constantly
        std::this_thread::sleep_for(std::chrono::milliseconds(SYNC_SERVER_STATE_SLEEP));
    }
}

sf::Uint32 NetworkingClient::getClientId() const {
    return mClientId;
}

sf::Uint32 NetworkingClient::getGameStateIsFresh() const {
    return mGameStateIsFresh;
}

void NetworkingClient::incrementTick() {
    mTick++;
}

unsigned int NetworkingClient::getTick() const {
    return static_cast<unsigned int>(mTick);
}

void NetworkingClient::setTick(unsigned int tick) {
    mTick = tick;
}
