#include "NetworkingClient.hpp"

#include <SFML/Network.hpp>
#include <chrono>
#include <ctime>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "../../common/util/InputDef.hpp"
#include "../../common/util/game_settings.hpp"
#include "../../common/util/network_util.hpp"

const sf::Time CLIENT_TCP_TIMEOUT = sf::milliseconds(300);
;

NetworkingClient::NetworkingClient(const std::string& server_ip) :
    m_gameState_t({}), m_serverIp(server_ip) {
    std::cout << "Starting client." << std::endl;

    createTcpSocket(SERVER_TCP_PORT);
    createUdpSocket();

    // m_reliableRecv = std::thread(&NetworkingClient::reliableRecv, this);
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
    // m_reliableRecv.join();
    m_reliableSend.join();
    m_unreliableRecv.join();
    m_unreliableSend.join();
}

/* Main thread methods */

void NetworkingClient::createTcpSocket(unsigned short port) {
    std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
    m_tcpSocket_t = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket);

    m_tcpSocket_t->connect(sf::IpAddress(m_serverIp), port);
}

void NetworkingClient::createUdpSocket() {
    std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
    m_udpSocket_t = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    m_udpSocket_t->bind(sf::Socket::AnyPort);
    m_udpSocket_t->setBlocking(false);
}

/**
 * The player id isn't ready at registration time so this function stalls until player id is
 * recieved.
 */
uint32_t NetworkingClient::registerClientAndFetchPlayerId() {
    registerClient();
    return fetchPlayerId();
}

/**
 * Keep asking server for player_id until it returns a response.
 * Server won't return a response if the player id isn't ready.
 */
uint32_t NetworkingClient::fetchPlayerId() {
    std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);

    while (true) {
        // Send request
        sf::Packet packet;
        if (packet << (sf::Uint32)ReliableCommandType::player_id) {
            m_tcpSocket_t->send(packet);
        } else {
            std::cout << "Failed to form packet" << std::endl;
            throw std::runtime_error("Unable to get player id.");
        }

        // Recieve response
        sf::Socket::Status status;
        sf::Packet reliable_response;
        ReliableCommand reliable_command;
        status = receiveWithTimeout(*m_tcpSocket_t, reliable_response, CLIENT_TCP_TIMEOUT);
        if (status == sf::Socket::Done) {
            reliable_response >> reliable_command;
            if (reliable_command.command == ReliableCommandType::player_id) {
                sf::Uint32 player_id;
                reliable_response >> player_id;
                return static_cast<uint32_t>(player_id);
            } else {
                std::cout << "Unexpected reliable command type." << std::endl;
                throw std::runtime_error("Unable to get player id.");
            }
        }
        std::this_thread::sleep_for(CLIENT_FETCH_PLAYER_ID_SLEEP);
    }
}

void NetworkingClient::registerClient() {
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
        throw std::runtime_error("Failed to register.");
    }
}

void NetworkingClient::readRegistrationResponse() {
    sf::Packet registration_response;
    ReliableCommand reliable_command;

    if (m_tcpSocket_t->receive(registration_response) == sf::Socket::Done) {
        if (registration_response >> reliable_command &&
            reliable_command.command == ReliableCommandType::register_client) {
            m_clientId_ta = static_cast<uint>(reliable_command.client_id);
            m_tick_ta = static_cast<uint>(reliable_command.tick);
            registration_response >> m_serverUdpPort;
            std::cout << "Registered as client " << m_clientId_ta << " at tick " << m_tick_ta
                      << std::endl
                      << "Server UDP port is: " << m_serverUdpPort << std::endl;
            return;
        }
    }
    throw std::runtime_error("Failed to register.");
}

GameState NetworkingClient::getGameState() {
    std::lock_guard<std::mutex> m_gameState_guard(m_gameState_lock);
    m_gameStateIsFresh_ta = false;
    return m_gameState_t;
}

void NetworkingClient::pushUnreliableInput(InputDef::UnreliableInput unreliable_input) {
    std::lock_guard<std::mutex> m_unreliableInputs_guard(m_unreliableInputs_lock);
    m_unreliableInputs_t.push(unreliable_input);
}

void NetworkingClient::pushReliableInput(InputDef::ReliableInput reliable_input) {
    std::lock_guard<std::mutex> m_reliableInputs_guard(m_reliableInputs_lock);
    m_reliableInputs_t.push(reliable_input);
}

int NetworkingClient::getClientId() const {
    return m_clientId_ta;
}

bool NetworkingClient::getGameStateIsFresh() const {
    return m_gameStateIsFresh_ta;
}

void NetworkingClient::incrementTick() {
    m_tick_ta++;
}

uint NetworkingClient::getTick() const {
    return m_tick_ta;
}

void NetworkingClient::setTick(uint tick) {
    m_tick_ta = tick;
}

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
            std::cout << "Unexpected reliable command type." << std::endl;
        }

        std::this_thread::sleep_for(CLIENT_RELIBALE_RECV_SLEEP);
    }
}

/* m_reliableSend thread methods */

void NetworkingClient::reliableSend() {
    // TODO(souren|#59): Don't spam server with TCP calls, optimize when updates are sent
    while (!m_stopThreads_ta) {
        sendReliableInput();

        std::this_thread::sleep_for(CLIENT_RELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendReliableInput() {
    std::lock_guard<std::mutex> m_reliableInputs_guard(m_reliableInputs_lock);

    if (m_reliableInputs_t.empty()) {
        return;
    }

    InputDef::ReliableInput reliable_input = m_reliableInputs_t.front();
    m_reliableInputs_t.pop();

    std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
    sf::Packet packet;
    if (packet << ReliableCommandType::reliable_input && packet << reliable_input) {
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
            GameState game_state;
            packet >> game_state;
            std::lock_guard<std::mutex> m_gameState_guard(m_gameState_lock);
            m_gameState_t = game_state;
            m_gameStateIsFresh_ta = true;
        }

        std::this_thread::sleep_for(CLIENT_UNRELIABLE_RECV_SLEEP);
    }
}

// m_unreliableSend Thread Methods

void NetworkingClient::unreliableSend() {
    while (!m_stopThreads_ta) {
        if (m_serverUdpPort != 0) {
            sendUnreliableInput();
        }

        std::this_thread::sleep_for(CLIENT_UNRELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendUnreliable(sf::Packet packet) {
    std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
    sf::Socket::Status status = sf::Socket::Partial;

    while (status == sf::Socket::Partial) {
        status = m_udpSocket_t->send(packet, sf::IpAddress(m_serverIp), m_serverUdpPort);
    }
}
void NetworkingClient::sendNatPunch() {
    sf::Packet packet;
    sf::Uint32 unreliable_input_cmd = UnreliableCommandType::nat_punch;
    UnreliableCommand unreliable_command = {(sf::Uint32)m_clientId_ta, unreliable_input_cmd,
                                            m_tick_ta};

    if (packet << unreliable_command) {
        sendUnreliable(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

void NetworkingClient::sendUnreliableInput() {
    std::lock_guard<std::mutex> m_unreliableInputs_guard(m_unreliableInputs_lock);
    if (m_unreliableInputs_t.empty()) {
        time_t seconds_passed = time(0) - m_lastSentNatPunch;
        if (seconds_passed > NAT_PUNCH_INTERVAL) {
            // keep the NAT tables populated by sending a nat punch noop to the server
            sendNatPunch();
            m_lastSentNatPunch = time(0);
        }
        return;
    }

    InputDef::UnreliableInput unreliable_input = m_unreliableInputs_t.front();
    m_unreliableInputs_t.pop();

    sf::Packet packet;
    sf::Uint32 unreliable_input_cmd = UnreliableCommandType::unreliable_input;
    UnreliableCommand unreliable_command = {(sf::Uint32)m_clientId_ta, unreliable_input_cmd,
                                            m_tick_ta};

    if (packet << unreliable_command && packet << unreliable_input) {
        sendUnreliable(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}
