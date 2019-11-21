#include "NetworkingServer.hpp"
#include <atomic>
#include <memory>
#include <utility>
#include <vector>

#include "../../common/events/ClientConnectedEvent.hpp"
#include "../../common/events/ClientDisconnectedEvent.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/events/PlayerCreatedEvent.hpp"
#include "../../common/util/InputDef.hpp"
#include "../../common/util/game_settings.hpp"
#include "../../common/util/network_util.hpp"

sf::Time TCP_TIMEOUT = sf::milliseconds(100);
;

NetworkingServer::NetworkingServer() : m_gameState_t() {
    std::cout << "Starting ungroup game server." << std::endl;

    createUdpSocket();
    addEventListeners();

    m_reliableRecvSend = std::thread(&NetworkingServer::reliableRecvSend, this);
    m_unreliableRecv = std::thread(&NetworkingServer::unreliableRecv, this);
    m_broadcastGameStateThread = std::thread(&NetworkingServer::broadcastGameState, this);
}

NetworkingServer::~NetworkingServer() {
    m_playerReliableUpdates_lock.unlock();
    m_playerUnreliableUpdates_lock.unlock();
    m_gameState_lock.unlock();
    EventController::getInstance().unlock();

    m_stopThreads_ta = true;

    m_reliableRecvSend.join();
    m_unreliableRecv.join();
    m_broadcastGameStateThread.join();
}

/* Main thread methods */

void NetworkingServer::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_PLAYER_CREATED,
        std::bind(&NetworkingServer::handlePlayerCreatedEvent, this, std::placeholders::_1));
}

void NetworkingServer::createUdpSocket() {
    std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
    m_udpSocket_t = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    m_udpSocket_t->bind(sf::Socket::AnyPort);
    m_udpSocket_t->setBlocking(false);
}

InputDef::PlayerInputs NetworkingServer::collectClientInputs() {
    // Give clients a window to write inputs
    {
        m_playerReliableUpdates_lock.unlock();
        m_playerUnreliableUpdates_lock.unlock();
        m_gameState_lock.unlock();
        EventController::getInstance().unlock();
        std::this_thread::sleep_for(SERVER_INPUT_WINDOW_SLEEP);
        m_playerReliableUpdates_lock.lock();
        m_playerUnreliableUpdates_lock.lock();
        m_gameState_lock.lock();
        EventController::getInstance().lock();
    }

    // Get client inputs
    InputDef::PlayerInputs pi = {
        m_playerUnreliableUpdates_t,
        m_playerReliableUpdates_t,
    };

    m_playerUnreliableUpdates_t.clear();
    m_playerReliableUpdates_t.clear();

    return pi;
}

void NetworkingServer::handlePlayerCreatedEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<PlayerCreatedEvent> player_created_event =
        std::dynamic_pointer_cast<PlayerCreatedEvent>(event);
    m_clientToPlayerIds_t[player_created_event->getClientId()] =
        player_created_event->getPlayerId();
}

void NetworkingServer::setState(const GameState& gs) {
    m_gameState_t = gs;
}

void NetworkingServer::incrementTick() {
    m_tick_ta++;
}

unsigned int NetworkingServer::getTick() const {
    return static_cast<unsigned int>(m_tick_ta);
}

void NetworkingServer::setTick(unsigned int tick) {
    m_tick_ta = tick;
}

/* UnreliableServer thread methods */

void NetworkingServer::unreliableRecv() {
    while (!m_stopThreads_ta) {
        sf::IpAddress sender;
        unsigned short port;
        sf::Packet command_packet;
        sf::Socket::Status status;

        {
            std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
            status = m_udpSocket_t->receive(command_packet, sender, port);
        }
        handleUnreliableCommand(status, command_packet, sender, port);

        std::this_thread::sleep_for(SERVER_UNRELIABLE_RECV_SLEEP);
    }
}

void NetworkingServer::handleUnreliableCommand(sf::Socket::Status status, sf::Packet command_packet,
                                               sf::IpAddress& sender, unsigned short port) {
    if (status == sf::Socket::NotReady) {
        return;
    }
    UnreliableCommand unreliable_command;
    command_packet >> unreliable_command;
    switch (unreliable_command.command) {
        case (sf::Uint32)UnreliableCommandType::unreliable_input: {
            setClientUnreliableUpdate(command_packet, unreliable_command.client_id,
                                      unreliable_command.tick);
            break;
        }
        default: {
            std::cout << "Unknown command code sent: " << unreliable_command.command << std::endl;
            break;
        }
    }
}

void NetworkingServer::setClientUnreliableUpdate(sf::Packet packet, int client_id,
                                                 uint client_tick) {
    int drift = std::abs(static_cast<int>((m_tick_ta - client_tick)));
    if (drift < CMD_DRIFT_THRESHOLD) {
        sf::Uint32 player_id;
        {
            std::lock_guard<std::mutex> m_clientToPlayerIds_guard(m_clientToPlayerIds_lock);
            if (m_clientToPlayerIds_t.count(client_id) == 0) {
                return;
            }
            player_id = m_clientToPlayerIds_t[client_id];
        }
        InputDef::UnreliableInput unreliable_input;
        packet >> unreliable_input;
        InputDef::PlayerUnreliableInput player_unreliable_input = {player_id, unreliable_input};
        {
            std::lock_guard<std::mutex> m_playerUnreliableUpdates_guard(
                m_playerUnreliableUpdates_lock);
            m_playerUnreliableUpdates_t.push_back(player_unreliable_input);
        }
    } else {
        std::cout << "Receive client_update command with tick drifted past drift threshold. "
                  << "Drift value is: " << drift << std::endl;
    }
}

/* ReliableServer thread methods */

void NetworkingServer::reliableRecvSend() {
    // Create a socket to listen to new connections
    sf::TcpListener listener;
    // Reliable socket
    listener.listen(SERVER_TCP_PORT);

    // Create a selector
    sf::SocketSelector selector;

    // Add the listener to the selector
    selector.add(listener);

    // Endless loop that waits for new connections
    while (!m_stopThreads_ta) {
        // Make the selector wait for data on any socket
        if (selector.wait(TCP_TIMEOUT)) {
            // Test the listener
            if (selector.isReady(listener)) {
                // The listener is ready: there is a pending connection
                auto socket = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket());
                if (listener.accept(*socket) == sf::Socket::Done) {
                    selector.add(*socket);
                    m_clients.push_back(std::make_pair(m_clientIdCounter++, std::move(socket)));
                } else {
                    socket.reset();
                }
            } else {
                // The listener socket is not ready, test all other sockets (the clients)
                for (auto& client : m_clients) {
                    sf::Uint32 client_id = client.first;
                    sf::TcpSocket& socket = *client.second;
                    if (selector.isReady(socket)) {
                        // The client has sent some data, we can receive it
                        sf::Packet command_packet;
                        sf::Socket::Status status = socket.receive(command_packet);
                        handleReliableCommand(status, command_packet, selector, socket, client_id);
                    }
                }
            }
        }

        std::this_thread::sleep_for(SERVER_RELIABLE_REVC_SEND_SLEEP);
    }
}

void NetworkingServer::handleReliableCommand(sf::Socket::Status status, sf::Packet command_packet,
                                             sf::SocketSelector& selector, sf::TcpSocket& socket,
                                             sf::Uint32 client_id) {
    sf::Uint32 reliable_command_type;
    switch (status) {
        case sf::Socket::Done:
            if (command_packet >> reliable_command_type) {
                if (reliable_command_type == (sf::Uint32)ReliableCommandType::register_client) {
                    registerClient(command_packet, socket, client_id);
                } else if (reliable_command_type == (sf::Uint32)ReliableCommandType::player_id) {
                    sendPlayerId(socket, client_id);
                } else if (reliable_command_type ==
                           (sf::Uint32)ReliableCommandType::reliable_input) {
                    setClientReliableUpdate(command_packet, client_id);
                }
            }
            break;
        case sf::TcpSocket::Error:
        case sf::TcpSocket::Disconnected:
            std::cout << "Removing client due to TCP dsconnect/error." << std::endl;
            clientDisconnect(socket, client_id);
            selector.remove(socket);
            break;
        default:
            std::cout << "TCP client sent unkown signal." << std::endl;
            break;
    }
}

void NetworkingServer::clientDisconnect(sf::TcpSocket& client, sf::Uint32 client_id) {
    // TODO(sourenp): Remove client from m_clients
    std::lock_guard<std::mutex> m_clientToPlayerIds_guard(m_clientToPlayerIds_lock);
    sf::Uint32 player_id = m_clientToPlayerIds_t[client_id];
    EventController::getInstance().queueEvent(std::shared_ptr<ClientDisconnectedEvent>(
        new ClientDisconnectedEvent(client_id, player_id)));
}

/**
 * Send player id if it's available.
 */
void NetworkingServer::sendPlayerId(sf::TcpSocket& socket, sf::Uint32 client_id) {
    sf::Uint32 player_id;
    {
        std::lock_guard<std::mutex> m_clientToPlayerIds_guard(m_clientToPlayerIds_lock);
        if (m_clientToPlayerIds_t.count(client_id) == 0) {
            return;
        }
        player_id = m_clientToPlayerIds_t[client_id];
    }
    sf::Packet packet;
    sf::Uint32 player_id_cmd = ReliableCommandType::player_id;
    ReliableCommand reliable_command = {client_id, player_id_cmd, m_tick_ta};
    if (packet << reliable_command << player_id) {
        socket.send(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

void NetworkingServer::setClientReliableUpdate(sf::Packet packet, int client_id) {
    uint32_t player_id;
    {
        std::lock_guard<std::mutex> m_clientToPlayerIds_guard(m_clientToPlayerIds_lock);
        player_id = m_clientToPlayerIds_t[client_id];
    }
    InputDef::ReliableInput reliable_input;
    packet >> reliable_input;
    InputDef::PlayerReliableInput player_reliable_input = {player_id, reliable_input};
    {
        std::lock_guard<std::mutex> m_playerReliableUpdates_guard(m_playerReliableUpdates_lock);
        m_playerReliableUpdates_t.push_back(player_reliable_input);
    }
}

void NetworkingServer::registerClient(sf::Packet packet, sf::TcpSocket& socket,
                                      sf::Uint32 client_id) {
    // Save client udp port
    sf::Uint16 client_udp_port;
    packet >> client_udp_port;
    {
        std::lock_guard<std::mutex> m_clientToUdpPorts_guard(m_clientToUdpPorts_lock);
        m_clientToUdpPorts_t[client_id] = client_udp_port;
    }

    // Send a response
    sf::Uint16 server_udp_port;
    {
        std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
        server_udp_port = m_udpSocket_t->getLocalPort();
    }
    sf::Packet response_packet;
    sf::Uint32 register_cmd = (sf::Uint32)ReliableCommandType::register_client;
    ReliableCommand reliable_command = {client_id, register_cmd, (sf::Uint32)m_tick_ta};
    if (response_packet << reliable_command << server_udp_port) {
        socket.send(response_packet);
        EventController::getInstance().queueEvent(
            std::shared_ptr<ClientConnectedEvent>(new ClientConnectedEvent(client_id)));
        std::cout << "Received client registration. Issued client ID " << client_id << std::endl;
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

/* UnreliableServerSend thread methods */

void NetworkingServer::broadcastGameState() {
    while (!m_stopThreads_ta) {
        sendGameState();
        std::this_thread::sleep_for(SERVER_BROADCAST_GAME_STATE_SLEEP);
    }
}

void NetworkingServer::sendGameState() {
    GameState game_state;
    {
        std::lock_guard<std::mutex> m_gameState_guard(m_gameState_lock);
        game_state = m_gameState_t;
    }
    sf::Packet packet = pack_game_state(game_state);
    {
        std::lock_guard<std::mutex> m_udpSocket_guard(m_udpSocket_lock);
        std::lock_guard<std::mutex> m_clientToUdpPorts_guard(m_clientToUdpPorts_lock);

        for (auto& it : m_clientToUdpPorts_t) {
            sf::Uint16& client_udp_port = it.second;
            sf::Socket::Status status = sf::Socket::Partial;
            while (status == sf::Socket::Partial) {
                status = m_udpSocket_t->send(packet, CLIENT_IP, client_udp_port);
            }
        }
    }
}
