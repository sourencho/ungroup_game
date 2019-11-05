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

NetworkingClient::NetworkingClient() : m_GameState_t() {
    std::cout << "Starting client..." << std::endl;

    createTcpSocket(SERVER_TCP_PORT);
    createUdpSocket();

    bool registered = registerNetworkingClient();
    if (registered) {
        std::cout << "Registered as client " << m_ClientId_ta << " at tick " << m_Tick_ta
                  << std::endl;
    } else {
        std::cout << "Failed to register." << std::endl;
    }

    m_ReliableRecv = std::thread(&NetworkingClient::reliableRecv, this);
    m_ReliableSend = std::thread(&NetworkingClient::reliableSend, this);
    m_UnreliableRecv = std::thread(&NetworkingClient::unreliableRecv, this);
    m_UnreliableSend = std::thread(&NetworkingClient::unreliableSend, this);
}

NetworkingClient::~NetworkingClient() {
    {
        std::lock_guard<std::mutex> m_TcpSocket_guard(m_TcpSocket_lock);
        m_TcpSocket_t->disconnect();
    }

    m_StopThreads_ta = true;
    m_ReliableRecv.join();
    m_ReliableSend.join();
    m_UnreliableRecv.join();
    m_UnreliableSend.join();
}

/* Main thread methods */

void NetworkingClient::createTcpSocket(unsigned short port) {
    std::lock_guard<std::mutex> m_TcpSocket_guard(m_TcpSocket_lock);
    m_TcpSocket_t = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket);
    m_TcpSocket_t->connect(SERVER_IP, port);
}

void NetworkingClient::createUdpSocket() {
    std::lock_guard<std::mutex> m_UdpSocket_guard(m_UdpSocket_lock);
    m_UdpSocket_t = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    m_UdpSocket_t->bind(sf::Socket::AnyPort);
    m_UdpSocket_t->setBlocking(false);
}

bool NetworkingClient::registerNetworkingClient() {
    sf::Packet registration_request;
    sf::Uint16 udp_port;
    {
        std::lock_guard<std::mutex> m_UdpSocket_guard(m_UdpSocket_lock);
        udp_port = m_UdpSocket_t->getLocalPort();
    }
    if (registration_request << ReliableCommandType::register_client << udp_port) {
        std::lock_guard<std::mutex> m_TcpSocket_guard(m_TcpSocket_lock);
        m_TcpSocket_t->send(registration_request);
        return readRegistrationResponse();
    } else {
        std::cout << "Failed to form packet" << std::endl;
        return false;
    }
}

bool NetworkingClient::readRegistrationResponse() {
    sf::Packet registration_response;
    ReliableCommand reliable_command;

    if (m_TcpSocket_t->receive(registration_response) == sf::Socket::Done) {
        if (registration_response >> reliable_command &&
            reliable_command.command == ReliableCommandType::register_client) {
            m_ClientId_ta = static_cast<uint>(reliable_command.client_id);
            m_Tick_ta = static_cast<uint>(reliable_command.tick);
            registration_response >> m_ServerUdpPort;
            return true;
        }
    }
    return false;
}

int NetworkingClient::getPlayerId() const { return m_PlayerId_ta; }

GameState NetworkingClient::getGameState() {
    std::lock_guard<std::mutex> m_GameState_guard(m_GameState_lock);
    m_GameStateIsFresh_ta = false;
    return m_GameState_t;
}

void NetworkingClient::setClientUnreliableUpdate(ClientUnreliableUpdate client_unreliable_update) {
    std::lock_guard<std::mutex> m_ClientUnreliableUpdate_guard(m_ClientUnreliableUpdate_lock);
    m_ClientUnreliableUpdate_t = client_unreliable_update;
}

void NetworkingClient::setClientReliableUpdate(ClientReliableUpdate client_reliable_update) {
    std::lock_guard<std::mutex> m_ClientReliableUpdate_guard(m_ClientReliableUpdate_lock);
    m_ClientReliableUpdate_t = client_reliable_update;
}

int NetworkingClient::getClientId() const { return m_ClientId_ta; }

bool NetworkingClient::getGameStateIsFresh() const { return m_GameStateIsFresh_ta; }

void NetworkingClient::incrementTick() { m_Tick_ta++; }

uint NetworkingClient::getTick() const { return m_Tick_ta; }

void NetworkingClient::setTick(uint tick) { m_Tick_ta = tick; }

/* m_ReliableRecv thread methods */

void NetworkingClient::reliableRecv() {
    while (!m_StopThreads_ta) {
        sf::Socket::Status status;
        sf::Packet reliable_response;
        ReliableCommand reliable_command;
        {
            std::lock_guard<std::mutex> m_TcpSocket_guard(m_TcpSocket_lock);
            status = receiveWithTimeout(*m_TcpSocket_t, reliable_response, CLIENT_TCP_TIMEOUT);
        }
        if (status == sf::Socket::Done) {
            reliable_response >> reliable_command;
            if (reliable_command.command == ReliableCommandType::player_id) {
                sf::Uint32 player_id;
                reliable_response >> player_id;
                m_PlayerId_ta = static_cast<uint>(player_id);
            } else {
                std::cout << "Unknown reliable command type." << std::endl;
            }
        }

        std::this_thread::sleep_for(CLIENT_RELIBALE_RECV_SLEEP);
    }
}

/* m_ReliableSend thread methods */

void NetworkingClient::reliableSend() {
    // TODO(souren|#59): Don't spam server with TCP calls, optimize when updates are sent
    while (!m_StopThreads_ta) {
        sendPlayerIdRequest();
        sendClientReliableUpdate();

        std::this_thread::sleep_for(CLIENT_RELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendPlayerIdRequest() {
    std::lock_guard<std::mutex> m_TcpSocket_guard(m_TcpSocket_lock);

    if (m_PlayerId_ta == -1) {
        sf::Packet packet;
        if (packet << (sf::Uint32)ReliableCommandType::player_id) {
            m_TcpSocket_t->send(packet);
        } else {
            std::cout << "Failed to form packet" << std::endl;
        }
    }
}

void NetworkingClient::sendClientReliableUpdate() {
    std::lock_guard<std::mutex> m_ClientReliableUpdate_guard(m_ClientReliableUpdate_lock);
    std::lock_guard<std::mutex> m_TcpSocket_guard(m_TcpSocket_lock);

    sf::Packet packet;
    if (packet << ReliableCommandType::client_reliable_update && packet
                                                                     << m_ClientReliableUpdate_t) {
        m_TcpSocket_t->send(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

/* m_UnreliableRecv thread methods */

void NetworkingClient::unreliableRecv() {
    while (!m_StopThreads_ta) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        sf::Socket::Status status;

        {
            std::lock_guard<std::mutex> m_UdpSocket_guard(m_UdpSocket_lock);
            status = m_UdpSocket_t->receive(packet, sender, port);
        }

        if (status != sf::Socket::NotReady) {
            GameState game_state = unpack_game_state(packet);
            m_GameStateIsFresh_ta = true;
            std::lock_guard<std::mutex> m_GameState_guard(m_GameState_lock);
            m_GameState_t = game_state;
        }

        std::this_thread::sleep_for(CLIENT_UNRELIABLE_RECV_SLEEP);
    }
}

// m_UnreliableSend Thread Methods

void NetworkingClient::unreliableSend() {
    while (!m_StopThreads_ta) {
        if (m_ServerUdpPort != 0) {
            sendClientUnreliableUpdate();
        }

        std::this_thread::sleep_for(CLIENT_UNRELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendClientUnreliableUpdate() {
    sf::Packet packet;
    sf::Uint32 client_unreliable_update_cmd = UnreliableCommandType::client_unreliable_update;
    UnreliableCommand unreliable_command = {(sf::Uint32)m_ClientId_ta, client_unreliable_update_cmd,
                                            m_Tick_ta};

    std::lock_guard<std::mutex> m_ClientReliableUpdate_guard(m_ClientReliableUpdate_lock);
    if (packet << unreliable_command && packet << m_ClientUnreliableUpdate_t) {
        std::lock_guard<std::mutex> m_UdpSocket_guard(m_UdpSocket_lock);
        sf::Socket::Status status = sf::Socket::Partial;
        while (status == sf::Socket::Partial) {
            status = m_UdpSocket_t->send(packet, SERVER_IP, m_ServerUdpPort);
        }
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}