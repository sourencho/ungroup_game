#include "NetworkingClient.hpp"

#include <SFML/Network.hpp>
#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

#include "../../common/util/game_settings.hpp"
#include "../../common/util/network_util.hpp"

const sf::Time CLIENT_TCP_TIMEOUT = sf::milliseconds(100);
;

NetworkingClient::NetworkingClient() : mGameState_t() {
    std::cout << "Starting client..." << std::endl;

    createTcpSocket(SERVER_TCP_PORT);
    createUdpSocket();

    bool registered = registerNetworkingClient();
    if (registered) {
        std::cout << "Registered as client " << mClientId_ta << " at tick " << mTick_ta
                  << std::endl;
    } else {
        std::cout << "Failed to register." << std::endl;
    }

    mReliableRecv = std::thread(&NetworkingClient::reliableRecv, this);
    mReliableSend = std::thread(&NetworkingClient::reliableSend, this);
    mUnreliableRecv = std::thread(&NetworkingClient::unreliableRecv, this);
    mUnreliableSend = std::thread(&NetworkingClient::unreliableSend, this);
}

NetworkingClient::~NetworkingClient() {
    {
        std::lock_guard<std::mutex> mTcpSocket_guard(mTcpSocket_lock);
        mTcpSocket_t->disconnect();
    }

    mStopThreads_ta = true;
    mReliableRecv.join();
    mReliableSend.join();
    mUnreliableRecv.join();
    mUnreliableSend.join();
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
    mUdpSocket_t->bind(sf::Socket::AnyPort);
    mUdpSocket_t->setBlocking(false);
}

bool NetworkingClient::registerNetworkingClient() {
    sf::Packet registration_request;
    sf::Uint16 udp_port;
    {
        std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
        udp_port = mUdpSocket_t->getLocalPort();
    }
    if (registration_request << ReliableCommandType::register_client << udp_port) {
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
            registration_response >> mServerUdpPort;
            return true;
        }
    }
    return false;
}

int NetworkingClient::getPlayerId() const { return mPlayerId_ta; }

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

int NetworkingClient::getClientId() const { return mClientId_ta; }

bool NetworkingClient::getGameStateIsFresh() const { return mGameStateIsFresh_ta; }

void NetworkingClient::incrementTick() { mTick_ta++; }

uint NetworkingClient::getTick() const { return mTick_ta; }

void NetworkingClient::setTick(uint tick) { mTick_ta = tick; }

/* mReliableRecv thread methods */

void NetworkingClient::reliableRecv() {
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
                sf::Uint32 player_id;
                reliable_response >> player_id;
                mPlayerId_ta = static_cast<uint>(player_id);
            } else {
                std::cout << "Unknown reliable command type." << std::endl;
            }
        }

        std::this_thread::sleep_for(CLIENT_RELIBALE_RECV_SLEEP);
    }
}

/* mReliableSend thread methods */

void NetworkingClient::reliableSend() {
    // TODO(souren|#59): Don't spam server with TCP calls, optimize when updates are sent
    while (!mStopThreads_ta) {
        sendPlayerIdRequest();
        sendClientReliableUpdate();

        std::this_thread::sleep_for(CLIENT_RELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendPlayerIdRequest() {
    std::lock_guard<std::mutex> mTcpSocket_guard(mTcpSocket_lock);

    if (mPlayerId_ta == -1) {
        sf::Packet packet;
        if (packet << (sf::Uint32)ReliableCommandType::player_id) {
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
    if (packet << ReliableCommandType::client_reliable_update && packet
                                                                     << mClientReliableUpdate_t) {
        mTcpSocket_t->send(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

/* mUnreliableRecv thread methods */

void NetworkingClient::unreliableRecv() {
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

        std::this_thread::sleep_for(CLIENT_UNRELIABLE_RECV_SLEEP);
    }
}

// mUnreliableSend Thread Methods

void NetworkingClient::unreliableSend() {
    while (!mStopThreads_ta) {
        if (mServerUdpPort != 0) {
            sendClientUnreliableUpdate();
        }

        std::this_thread::sleep_for(CLIENT_UNRELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendClientUnreliableUpdate() {
    sf::Packet packet;
    sf::Uint32 client_unreliable_update_cmd = UnreliableCommandType::client_unreliable_update;
    UnreliableCommand unreliable_command = {(sf::Uint32)mClientId_ta, client_unreliable_update_cmd,
                                            mTick_ta};

    std::lock_guard<std::mutex> mClientReliableUpdate_guard(mClientReliableUpdate_lock);
    if (packet << unreliable_command && packet << mClientUnreliableUpdate_t) {
        std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
        sf::Socket::Status status = sf::Socket::Partial;
        while (status == sf::Socket::Partial) {
            status = mUdpSocket_t->send(packet, SERVER_IP, mServerUdpPort);
        }
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}