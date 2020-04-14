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

NetworkingClient::NetworkingClient(const std::string& server_ip) :
    m_gameState_t({}), m_serverIp(server_ip) {
    std::cout << "Starting client." << std::endl;

    createTcpSocket(GAME_SETTINGS.SERVER_TCP_PORT);

    createInputUdpSocket();
    createStateUdpSocket();

    // m_reliableRecv = std::thread(&NetworkingClient::reliableRecv, this);
    m_reliableSend = std::thread(&NetworkingClient::reliableSend, this);
    m_unreliableRecv = std::thread(&NetworkingClient::unreliableRecv, this);
    m_unreliableSend = std::thread(&NetworkingClient::unreliableSend, this);
    m_natSend = std::thread(&NetworkingClient::natSend, this);
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
    m_natSend.join();
}

/* Main thread methods */

void NetworkingClient::createTcpSocket(unsigned short port) {
    std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
    m_tcpSocket_t = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket);
    m_tcpSocket_t->connect(sf::IpAddress(m_serverIp), port);
}

void NetworkingClient::createInputUdpSocket() {
    m_inputUdpSocket = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    m_inputUdpSocket->bind(sf::Socket::AnyPort);
}

void NetworkingClient::createStateUdpSocket() {
    m_stateUdpSocket = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    m_stateUdpSocket->bind(sf::Socket::AnyPort);
    m_stateUdpSocket->setBlocking(false);
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

    while (true) {
        // Send request
        sf::Packet packet;
        if (packet << (sf::Uint32)ReliableCommandType::player_id) {
            {
                std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
                m_tcpSocket_t->send(packet);
            }
        } else {
            std::cout << "Failed to form packet" << std::endl;
            throw std::runtime_error("Unable to get player id.");
        }

        // Recieve response
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
                return static_cast<uint32_t>(player_id);
            } else {
                std::cout << "Unexpected reliable command type." << std::endl;
                throw std::runtime_error("Unable to get player id.");
            }
        }
        std::this_thread::sleep_for(GAME_SETTINGS.CLIENT_FETCH_PLAYER_ID_SLEEP);
    }
}

void NetworkingClient::registerClient() {
    sf::Packet registration_request;
    sf::Uint16 udp_port = m_stateUdpSocket->getLocalPort();
    if (registration_request << ReliableCommandType::register_client << udp_port) {
        {
            std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
            m_tcpSocket_t->send(registration_request);
        }
        return readRegistrationResponse();
    } else {
        std::cout << "Failed to form packet" << std::endl;
        throw std::runtime_error("Failed to register.");
    }
}

void NetworkingClient::readRegistrationResponse() {
    sf::Packet registration_response;
    ReliableCommand reliable_command;
    sf::Socket::Status status;

    {
        std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
        status = m_tcpSocket_t->receive(registration_response);
    }

    if (status == sf::Socket::Done) {
        if (registration_response >> reliable_command &&
            reliable_command.command == ReliableCommandType::register_client) {
            m_clientId_ta = static_cast<uint>(reliable_command.client_id);
            m_tick_ta = static_cast<uint>(reliable_command.tick);
            registration_response >> m_serverInputUdpPort >> m_serverStateUdpPort;
            std::cout << "Registered as client " << m_clientId_ta << " at tick " << m_tick_ta
                      << std::endl
                      << "Server Input UDP port is: " << m_serverInputUdpPort << std::endl
                      << "Server State UDP port is: " << m_serverStateUdpPort << std::endl;
            return;
        }
    }
    throw std::runtime_error("Failed to register.");
}

GameState NetworkingClient::getLatestGameState() {
    std::lock_guard<std::mutex> m_gameState_guard(m_gameState_lock);
    return m_gameState_t;
}

uint NetworkingClient::getLatestGameStateTick() {
    std::lock_guard<std::mutex> m_gameState_guard(m_gameState_lock);
    return m_gameState_t.core.tick;
}

void NetworkingClient::pushUnreliableInput(InputDef::UnreliableInput unreliable_input) {
    std::lock_guard<std::mutex> m_unreliableInput_guard(m_unreliableInput_lock);
    m_unreliableInput_t = unreliable_input;
}

void NetworkingClient::pushReliableInput(InputDef::ReliableInput reliable_input) {
    std::lock_guard<std::mutex> m_reliableInputs_guard(m_reliableInputs_lock);
    m_reliableInputs_t.push(reliable_input);
}

int NetworkingClient::getClientId() const {
    return m_clientId_ta;
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

        std::this_thread::sleep_for(GAME_SETTINGS.CLIENT_RELIBALE_RECV_SLEEP);
    }
}

/* m_reliableSend thread methods */

void NetworkingClient::reliableSend() {
    // TODO(souren|#59): Don't spam server with TCP calls, optimize when updates are sent
    while (!m_stopThreads_ta) {
        sendReliableInput();

        std::this_thread::sleep_for(GAME_SETTINGS.CLIENT_RELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendReliableInput() {
    std::lock_guard<std::mutex> m_reliableInputs_guard(m_reliableInputs_lock);

    if (m_reliableInputs_t.empty()) {
        return;
    }

    InputDef::ReliableInput reliable_input = m_reliableInputs_t.front();
    m_reliableInputs_t.pop();

    sf::Packet packet;
    if (packet << ReliableCommandType::reliable_input && packet << reliable_input) {
        {
            std::lock_guard<std::mutex> m_tcpSocket_guard(m_tcpSocket_lock);
            m_tcpSocket_t->send(packet);
        }
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

/* m_unreliableRecv thread methods */

void NetworkingClient::unreliableRecv() {
    while (!m_stopThreads_ta) {
        // Recieve State
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        sf::Socket::Status status;

        {
            std::lock_guard<std::mutex> m_stateUdpSocket_guard(m_stateUdpSocket_lock);
            status = m_stateUdpSocket->receive(packet, sender, port);
        }

        if (status != sf::Socket::NotReady) {
            GameState game_state;
            packet >> game_state;
            std::lock_guard<std::mutex> m_gameState_guard(m_gameState_lock);
            m_gameState_t = game_state;
        }

        std::this_thread::sleep_for(GAME_SETTINGS.CLIENT_UNRELIABLE_RECV_SLEEP);
    }
}

// m_unreliableSend Thread Methods

void NetworkingClient::unreliableSend() {
    while (!m_stopThreads_ta) {
        if (m_serverInputUdpPort != 0) {
            sendUnreliableInput();
        }

        std::this_thread::sleep_for(GAME_SETTINGS.CLIENT_UNRELIABLE_SEND_SLEEP);
    }
}

void NetworkingClient::sendUnreliableInput() {
    std::lock_guard<std::mutex> m_unreliableInput_guard(m_unreliableInput_lock);

    sf::Packet packet;
    sf::Uint32 unreliable_input_cmd = UnreliableCommandType::unreliable_input;
    UnreliableCommand unreliable_command = {(sf::Uint32)m_clientId_ta, unreliable_input_cmd,
                                            m_tick_ta};

    if (packet << unreliable_command && packet << m_unreliableInput_t) {
        sf::Socket::Status status = sf::Socket::Partial;

        while (status == sf::Socket::Partial) {
            status =
                m_inputUdpSocket->send(packet, sf::IpAddress(m_serverIp), m_serverInputUdpPort);
        }
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

/* m_natSend thread methods */

void NetworkingClient::natSend() {
    while (!m_stopThreads_ta) {
        sendNatPunch();
        std::this_thread::sleep_for(GAME_SETTINGS.CLIENT_NAT_SEND_SLEEP);
    }
}

void NetworkingClient::sendNatPunch() {
    sf::Packet packet;
    sf::Uint32 unreliable_input_cmd = UnreliableCommandType::nat_punch;
    UnreliableCommand unreliable_command = {(sf::Uint32)m_clientId_ta, unreliable_input_cmd,
                                            m_tick_ta};
    if (packet << unreliable_command) {
        std::lock_guard<std::mutex> m_stateUdpSocket_guard(m_stateUdpSocket_lock);
        sf::Socket::Status status = sf::Socket::Partial;
        while (status == sf::Socket::Partial) {
            status =
                m_stateUdpSocket->send(packet, sf::IpAddress(m_serverIp), m_serverStateUdpPort);
        }
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}
