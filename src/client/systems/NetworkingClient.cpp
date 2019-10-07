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
sf::Time CLIENT_TCP_TIMEOUT = sf::milliseconds(100);;

unsigned short TCP_PORT = 4844;
unsigned short UDP_PORT = 4888;


NetworkingClient::NetworkingClient():mGameState_t() {
    std::cout << "Starting client..." << std::endl;

    createTcpSocket(TCP_PORT);
    createUdpSocket();

    bool registered = registerNetworkingClient();
    if (registered) {
        std::cout << "Registered as client " << mClientId_ta << "at tick " << mTick_ta << std::endl;
    } else {
        std::cout << "Failed to register." << std::endl;
    }

    mReliableClientRecv = std::thread(&NetworkingClient::reliableClientRecv, this);
    mReliableClientSend = std::thread(&NetworkingClient::reliableClientSend, this);
    mUnreliableClientRecv = std::thread(&NetworkingClient::unreliableClientRecv, this);
    mUnreliableClientSend = std::thread(&NetworkingClient::unreliableClientSend, this);
    mSyncServerState = std::thread(&NetworkingClient::syncServerState, this);
}

NetworkingClient::~NetworkingClient() {
    std::cout << "Deconsturcting NetworkingClient" << std::endl;
    mStopThreads_ta = true;
    std::cout << "Joining reliable recv thread..." << std::endl;
    mReliableClientRecv.join();
    std::cout << "Joining reliable send thread..." << std::endl;
    mReliableClientSend.join();
    std::cout << "Joining unreliable recv thread..." << std::endl;
    mUnreliableClientRecv.join();
    std::cout << "Joining unreliable send thread..." << std::endl;
    mUnreliableClientSend.join();
    std::cout << "Joining sync server state thread..." << std::endl;
    mSyncServerState.join();
    std::cout << "Done." << std::endl;
}

/* Main thread methods */

void NetworkingClient::createTcpSocket(unsigned short port) {
    std::lock_guard<std::mutex> mTcpSocket_guard(mTcpSocket_lock);
    mTcpSocket_t = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket);
    mTcpSocket_t->connect(SERVER_IP, port);
}

void NetworkingClient::createUdpSocket() {
    std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
    mUdpSocket_t = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    mUdpSocket_t->bind(0);
    mUdpSocket_t->setBlocking(false);
}

bool NetworkingClient::registerNetworkingClient() {
    sf::Packet registration_request;
    if (registration_request << ReliableCommandType::register_client) {
        std::lock_guard<std::mutex> mTcpSocket_guard(mTcpSocket_lock);
        mTcpSocket_t->send(registration_request);
        return readRegistrationResponse();
    } else {
        std::cout << "Failed to form packet" << std::endl;
        return false;
    }
}

bool NetworkingClient::readRegistrationResponse() {
    sf::Packet registration_response;
    ReliableCommand reliable_command;

    if (mTcpSocket_t->receive(registration_response) == sf::Socket::Done) {
        if (registration_response >> reliable_command &&
            reliable_command.command == ReliableCommandType::register_client) {
            mClientId_ta = static_cast<uint>(reliable_command.client_id);
            mTick_ta = static_cast<uint>(reliable_command.tick);
            return true;
        }
    }
    return false;
}

int NetworkingClient::getPlayerId() const {
    return mPlayerId_ta;
}

GameState NetworkingClient::getGameState() {
     std::lock_guard<std::mutex> mGameState_guard(mGameState_lock);
    mGameStateIsFresh_ta = false;
    return mGameState_t;
}

void NetworkingClient::setClientUnreliableUpdate(ClientUnreliableUpdate client_unreliable_update) {
    std::lock_guard<std::mutex> mClientUnreliableUpdate_guard(mClientUnreliableUpdate_lock);
    mClientUnreliableUpdate_t = client_unreliable_update;
}

void NetworkingClient::setClientReliableUpdate(ClientReliableUpdate client_reliable_update) {
    std::lock_guard<std::mutex> mClientReliableUpdate_guard(mClientReliableUpdate_lock);
    mClientReliableUpdate_t = client_reliable_update;
}

int NetworkingClient::getClientId() const {
    return mClientId_ta;
}

bool NetworkingClient::getGameStateIsFresh() const {
    return mGameStateIsFresh_ta;
}

void NetworkingClient::incrementTick() {
    mTick_ta++;
}

uint NetworkingClient::getTick() const {
    return mTick_ta;
}

void NetworkingClient::setTick(uint tick) {
    mTick_ta = tick;
}

/* mReliableClientRecv thread methods */

void NetworkingClient::reliableClientRecv() {
    while (!mStopThreads_ta) {
        sf::Socket::Status status;
        sf::Packet reliable_response;
        ReliableCommand reliable_command;
        {
            std::lock_guard<std::mutex> mTcpSocket_guard(mTcpSocket_lock);
            status = receiveWithTimeout(*mTcpSocket_t, reliable_response, CLIENT_TCP_TIMEOUT);
        }
        if (status == sf::Socket::Done) {
            reliable_response >> reliable_command;
            if (reliable_command.command == ReliableCommandType::player_id) {
                PlayerId pi;
                reliable_response >> pi;
                mPlayerId_ta = static_cast<uint>(pi.player_id);
            } else {
                std::cout << "Unknown reliable command type." << std::endl;
            }
        }
    }
}

/* mReliableClientSend thread methods */

void NetworkingClient::reliableClientSend() {
    // TODO(souren|#59): Don't spam server with TCP calls, optimize when updates are sent
    while (!mStopThreads_ta) {
        sendPlayerIdRequest();
        sendClientReliableUpdate();
        std::this_thread::sleep_for(std::chrono::milliseconds(RELIABLE_UPDATE_SEND_SLEEP));
    }
}

void NetworkingClient::sendPlayerIdRequest() {
    std::lock_guard<std::mutex> mTcpSocket_guard(mTcpSocket_lock);

    if (mPlayerId_ta == -1) {
        sf::Packet packet;
        if (packet << (sf::Uint32) ReliableCommandType::player_id) {
            mTcpSocket_t->send(packet);
        } else {
            std::cout << "Failed to form packet" << std::endl;
        }
    }
}

void NetworkingClient::sendClientReliableUpdate() {
    std::lock_guard<std::mutex> mClientReliableUpdate_guard(mClientReliableUpdate_lock);
    std::lock_guard<std::mutex> mTcpSocket_guard(mTcpSocket_lock);

    sf::Packet packet;
    if (packet << ReliableCommandType::client_reliable_update &&
        packet << mClientReliableUpdate_t) {
        mTcpSocket_t->send(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

/* mUnreliableClientRecv thread methods */

void NetworkingClient::unreliableClientRecv() {
    while (!mStopThreads_ta) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        sf::Socket::Status status;

        {
            std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
            status = mUdpSocket_t->receive(packet, sender, port);
        }

        if (status != sf::Socket::NotReady) {
            GameState game_state = unpack_game_state(packet);
            mGameStateIsFresh_ta = true;
            std::lock_guard<std::mutex> mGameState_guard(mGameState_lock);
            mGameState_t = game_state;
        }
    }
}

// mUnreliableClientSend Thread Methods

void NetworkingClient::unreliableClientSend() {
    while (!mStopThreads_ta) {
        sendClientUnreliableUpdate();
        std::this_thread::sleep_for(std::chrono::milliseconds(UNRELIABLE_UPDATE_SEND_SLEEP));
    }
}

void NetworkingClient::sendClientUnreliableUpdate() {
    sf::Packet packet;
    sf::Uint32 client_unreliable_update_cmd = UnreliableCommandType::client_unreliable_update;
    UnreliableCommand unreliable_command = {(sf::Uint32) mClientId_ta, client_unreliable_update_cmd,
        mTick_ta};

    std::lock_guard<std::mutex> mClientReliableUpdate_guard(mClientReliableUpdate_lock);
    if (packet << unreliable_command && packet << mClientUnreliableUpdate_t) {
        std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
        sf::Socket::Status status = sf::Socket::Partial;
        while (status == sf::Socket::Partial) {
            status = mUdpSocket_t->send(packet, SERVER_IP, UDP_PORT);
        }
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

/* mSyncServerState thread methods */

// this probably should just be in unreliableClientSend and unreliableClientSend
// should poll client state to see if the current tick has a move AND if the move has
// been sent to the server yet. If both are true, then it should try to communicate the move.
// This would decouple local move setting commands from the network communication. They'd be
// happening in different threads, which is a Good Thing(tm) to avoid blocking in the client on
// network IO.
void NetworkingClient::syncServerState() {
    while (!mStopThreads_ta) {
        sf::Packet packet;
        sf::Uint32 fetch_state_cmd = UnreliableCommandType::fetch_state;
        UnreliableCommand unreliable_command = {(sf::Uint32) mClientId_ta, fetch_state_cmd,
            mTick_ta};
        if (packet << unreliable_command) {
            std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
            sf::Socket::Status status = sf::Socket::Partial;
            while (status == sf::Socket::Partial) {
                status = mUdpSocket_t->send(packet, SERVER_IP, UDP_PORT);
            }
        } else {
            std::cout << "Failed to form packet" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SYNC_SERVER_STATE_SLEEP));
    }
}
