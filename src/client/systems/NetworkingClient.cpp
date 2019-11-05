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

NetworkingClient::NetworkingClient() : m_gameState_t() {
    std::cout << "Starting client..." << std::endl;

    createTcpSocket(SERVER_TCP_PORT);
    createUdpSocket();

    bool registered = registerNetworkingClient();
    if (registered) {
        std::cout << "Registered as client " << m_clientId_ta << " at tick " << m_tick_ta
                  << std::endl;
    } else {
        std::cout << "Failed to register." << std::endl;
    }

    m_reliableRecv = std::thread(&NetworkingClient::reliableRecv, this);
    m_reliableSend = std::thread(&NetworkingClient::reliableSend, this);
    m_unreliableRecv = std::thread(&NetworkingClient::unreliableRecv, this);
    m_unreliableSend = std::thread(&NetworkingClient::unreliableSend, this);
}

NetworkingClient::~NetworkingClient() {
    {
        std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
        m_tcpSocket_t->disconnect();
    }

    m_stopThreads_ta = true;
    m_reliableRecv.join();
    m_reliableSend.join();
    m_unreliableRecv.join();
    m_unreliableSend.join();
}

/* Main thread methods */

void NetworkingClient::createTcpSocket(unsigned short port) {
    std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
    m_tcpSocket_t = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket);
    m_tcpSocket_t->connect(SERVER_IP, port);
}

void NetworkingClient::createUdpSocket() {
    std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
    m_udpSocket_t = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    m_udpSocket_t->bind(sf::Socket::AnyPort);
    m_udpSocket_t->setBlocking(false);
}

bool NetworkingClient::registerNetworkingClient() {
    sf::Packet registration_request;
    sf::Uint16 udp_port;
    {
        std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
        udp_port = m_udpSocket_t->getLocalPort();
    }
    if (registration_request << ReliableCommandType::register_client << udp_port) {
        std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
        m_tcpSocket_t->send(registration_request);
        return readRegistrationResponse();
    } else {
        std::cout << "Failed to form packet" << std::endl;
        return false;
    }
}

bool NetworkingClient::readRegistrationResponse() {
    sf::Packet registration_response;
    ReliableCommand reliable_command;

    if (m_tcpSocket_t->receive(registration_response) == sf::Socket::Done) {
        if (registration_response >> reliable_command &&
            reliable_command.command == ReliableCommandType::register_client) {
            m_clientId_ta = static_cast<uint>(reliable_command.client_id);
            m_tick_ta = static_cast<uint>(reliable_command.tick);
            registration_response >> m_serverUdpPort;
            return true;
        }
    }
    return false;
}

int NetworkingClient::getPlayerId() const { return m_playerId_ta; }

GameState NetworkingClient::getGameState() {
    std::lock_guard<std::mutex> m_gameState_guard(m_gameState_lock);
    m_gameStateIsFresh_ta = false;
    return m_gameState_t;
}

void NetworkingClient::setClientUnreliableUpdate(ClientUnreliableUpdate client_unreliable_update) {
    std::lock_guard<std::mutex> m_clientUnreliableUpdate_guard(m_clientUnreliableUpdate_lock);
    m_clientUnreliableUpdate_t = client_unreliable_update;
}

void NetworkingClient::setClientReliableUpdate(ClientReliableUpdate client_reliable_update) {
    std::lock_guard<std::mutex> m_clientReliableUpdate_guard(m_clientReliableUpdate_lock);
    m_clientReliableUpdate_t = client_reliable_update;
}

int NetworkingClient::getClientId() const { return m_clientId_ta; }

bool NetworkingClient::getGameStateIsFresh() const { return m_gameStateIsFresh_ta; }

void NetworkingClient::incrementTick() { m_tick_ta++; }

uint NetworkingClient::getTick() const { return m_tick_ta; }

void NetworkingClient::setTick(uint tick) { m_tick_ta = tick; }

/* m_reliableRecv thread methods */

void NetworkingClient::reliableRecv() {
    while (!m_stopThreads_ta) {
        sf::Socket::Status status;
        sf::Packet reliable_response;
        ReliableCommand reliable_command;
        {
            std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
            status = receiveWithTimeout(*m_tcpSocket_t, reliable_response, CLIENT_TCP_TIMEOUT);
        }
        if (status == sf::Socket::Done) {
            reliable_response >> reliable_command;
            if (reliable_command.command == ReliableCommandType::player_id) {
                sf::Uint32 player_id;
                reliable_response >> player_id;
                m_playerId_ta = static_cast<uint>(player_id);
            } else {
                std::cout << "Unknown reliable command type." << std::endl;
            }
        }

        std::this_thread::sleep_for(CLIENT_RELIBALE_RECV_SLEEP);
    }
}

/* m_reliableSend thread methods */

void NetworkingClient::reliableSend() {
    // TODO(souren|#59): Don't spam server with TCP calls, optimize when updates are sent
    while (!m_stopThreads_ta) {
        sendPlayerIdRequest();
        sendClientReliableUpdate();

        std::this_thread::sleep_for(CLIENT_RELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendPlayerIdRequest() {
    std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);

    if (m_playerId_ta == -1) {
        sf::Packet packet;
        if (packet << (sf::Uint32)ReliableCommandType::player_id) {
            m_tcpSocket_t->send(packet);
        } else {
            std::cout << "Failed to form packet" << std::endl;
        }
    }
}

void NetworkingClient::sendClientReliableUpdate() {
    std::lock_guard<std::mutex> m_clientReliableUpdate_guard(m_clientReliableUpdate_lock);
    std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);

    sf::Packet packet;
    if (packet << ReliableCommandType::client_reliable_update && packet
                                                                     << m_clientReliableUpdate_t) {
        m_tcpSocket_t->send(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

/* m_unreliableRecv thread methods */

void NetworkingClient::unreliableRecv() {
    while (!m_stopThreads_ta) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        sf::Socket::Status status;

        {
            std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
            status = m_udpSocket_t->receive(packet, sender, port);
        }

        if (status != sf::Socket::NotReady) {
            GameState game_state = unpack_game_state(packet);
            m_gameStateIsFresh_ta = true;
            std::lock_guard<std::mutex> m_gameState_guard(m_gameState_lock);
            m_gameState_t = game_state;
        }

        std::this_thread::sleep_for(CLIENT_UNRELIABLE_RECV_SLEEP);
    }
}

// m_unreliableSend Thread Methods

void NetworkingClient::unreliableSend() {
    while (!m_stopThreads_ta) {
        if (m_serverUdpPort != 0) {
            sendClientUnreliableUpdate();
        }

        std::this_thread::sleep_for(CLIENT_UNRELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendClientUnreliableUpdate() {
    sf::Packet packet;
    sf::Uint32 client_unreliable_update_cmd = UnreliableCommandType::client_unreliable_update;
    UnreliableCommand unreliable_command = {(sf::Uint32)m_clientId_ta, client_unreliable_update_cmd,
                                            m_tick_ta};

    std::lock_guard<std::mutex> m_clientReliableUpdate_guard(m_clientReliableUpdate_lock);
    if (packet << unreliable_command && packet << m_clientUnreliableUpdate_t) {
        std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
        sf::Socket::Status status = sf::Socket::Partial;
        while (status == sf::Socket::Partial) {
            status = m_udpSocket_t->send(packet, SERVER_IP, m_serverUdpPort);
        }
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}