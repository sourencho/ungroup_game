#include "NetworkingServer.hpp"
#include <atomic>
#include <memory>
#include <utility>
#include <vector>

#include "../../common/events/ClientConnectedEvent.hpp"
#include "../../common/events/ClientDisconnectedEvent.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/util/game_settings.hpp"
#include "../../common/util/network_util.hpp"

sf::Time TCP_TIMEOUT = sf::milliseconds(100);
;

NetworkingServer::NetworkingServer() : m_GameState_t() {
    std::cout << "Starting ungroup game server." << std::endl;
    ;

    createUdpSocket();

    m_ReliableRecvSend = std::thread(&NetworkingServer::reliableRecvSend, this);
    m_UnreliableRecv = std::thread(&NetworkingServer::unreliableRecv, this);
    m_BroadcastGameStateThread = std::thread(&NetworkingServer::broadcastGameState, this);
}

NetworkingServer::~NetworkingServer() {
    m_ClientReliableUpdates_lock.unlock();
    m_ClientUnreliableUpdates_lock.unlock();
    m_GameState_lock.unlock();
    EventController::getInstance().unlock();

    m_StopThreads_ta = true;

    m_ReliableRecvSend.join();
    m_UnreliableRecv.join();
    m_BroadcastGameStateThread.join();
}

/* Main thread methods */

void NetworkingServer::createUdpSocket() {
    std::lock_guard<std::mutex> m_UdpSocket_guard(m_UdpSocket_lock);
    m_UdpSocket_t = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    m_UdpSocket_t->bind(sf::Socket::AnyPort);
    m_UdpSocket_t->setBlocking(false);
}

ClientInputs NetworkingServer::collectClientInputs() {
    // Give clients a window to write inputs
    {
        m_ClientReliableUpdates_lock.unlock();
        m_ClientUnreliableUpdates_lock.unlock();
        m_GameState_lock.unlock();
        EventController::getInstance().unlock();
        std::this_thread::sleep_for(SERVER_INPUT_WINDOW_SLEEP);
        m_ClientReliableUpdates_lock.lock();
        m_ClientUnreliableUpdates_lock.lock();
        m_GameState_lock.lock();
        EventController::getInstance().lock();
    }

    // Get client inputs
    ClientInputs cis = {
        m_ClientUnreliableUpdates_t,
        m_ClientReliableUpdates_t,
    };

    m_ClientUnreliableUpdates_t.clear();
    m_ClientReliableUpdates_t.clear();

    return cis;
}

void NetworkingServer::setClientToPlayerId(sf::Int32 client_id, int player_id) {
    m_ClientToPlayerIds_t[client_id] = player_id;
}

void NetworkingServer::setState(const GameState& gs) { m_GameState_t = gs; }

void NetworkingServer::incrementTick() { m_Tick_ta++; }

unsigned int NetworkingServer::getTick() const { return static_cast<unsigned int>(m_Tick_ta); }

void NetworkingServer::setTick(unsigned int tick) { m_Tick_ta = tick; }

/* UnreliableServer thread methods */

void NetworkingServer::unreliableRecv() {
    while (!m_StopThreads_ta) {
        sf::IpAddress sender;
        unsigned short port;
        sf::Packet command_packet;
        sf::Socket::Status status;

        {
            std::lock_guard<std::mutex> m_UdpSocket_guard(m_UdpSocket_lock);
            status = m_UdpSocket_t->receive(command_packet, sender, port);
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
        case (sf::Uint32)UnreliableCommandType::client_unreliable_update: {
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
    int drift = std::abs(static_cast<int>((m_Tick_ta - client_tick)));
    if (drift < CMD_DRIFT_THRESHOLD) {
        ClientUnreliableUpdate client_unreliable_update;
        packet >> client_unreliable_update;
        ClientIdAndUnreliableUpdate client_id_and_unreliable_update = {client_id,
                                                                       client_unreliable_update};
        {
            std::lock_guard<std::mutex> m_ClientUnreliableUpdates_guard(
                m_ClientUnreliableUpdates_lock);
            m_ClientUnreliableUpdates_t.push_back(client_id_and_unreliable_update);
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
    while (!m_StopThreads_ta) {
        // Make the selector wait for data on any socket
        if (selector.wait(TCP_TIMEOUT)) {
            // Test the listener
            if (selector.isReady(listener)) {
                // The listener is ready: there is a pending connection
                auto socket = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket());
                if (listener.accept(*socket) == sf::Socket::Done) {
                    selector.add(*socket);
                    m_Clients.push_back(std::make_pair(m_ClientIdCounter++, std::move(socket)));
                } else {
                    socket.reset();
                }
            } else {
                // The listener socket is not ready, test all other sockets (the clients)
                for (auto& client : m_Clients) {
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
                           (sf::Uint32)ReliableCommandType::client_reliable_update) {
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
    // TODO(sourenp): Remove client from m_Clients
    std::lock_guard<std::mutex> m_ClientToPlayerIds_guard(m_ClientToPlayerIds_lock);
    sf::Uint32 player_id = m_ClientToPlayerIds_t[client_id];
    EventController::getInstance().queueEvent(std::shared_ptr<ClientDisconnectedEvent>(
        new ClientDisconnectedEvent(client_id, player_id)));
}

void NetworkingServer::sendPlayerId(sf::TcpSocket& socket, sf::Uint32 client_id) {
    sf::Uint32 player_id;
    {
        std::lock_guard<std::mutex> m_ClientToPlayerIds_guard(m_ClientToPlayerIds_lock);
        if (m_ClientToPlayerIds_t.count(client_id) == 0) {
            return;
        }
        player_id = m_ClientToPlayerIds_t[client_id];
    }
    sf::Packet packet;
    sf::Uint32 player_id_cmd = ReliableCommandType::player_id;
    ReliableCommand reliable_command = {client_id, player_id_cmd, m_Tick_ta};
    if (packet << reliable_command << player_id) {
        socket.send(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

void NetworkingServer::setClientReliableUpdate(sf::Packet packet, int client_id) {
    ClientReliableUpdate client_reliable_update;
    packet >> client_reliable_update;
    ClientIdAndReliableUpdate client_id_and_reliable_update = {client_id, client_reliable_update};
    {
        std::lock_guard<std::mutex> m_ClientReliableUpdates_guard(m_ClientReliableUpdates_lock);
        m_ClientReliableUpdates_t.push_back(client_id_and_reliable_update);
    }
}

void NetworkingServer::registerClient(sf::Packet packet, sf::TcpSocket& client,
                                      sf::Uint32 client_id) {
    // Save client udp port
    sf::Uint16 client_udp_port;
    packet >> client_udp_port;
    {
        std::lock_guard<std::mutex> m_ClientToUdpPorts_guard(m_ClientToUdpPorts_lock);
        m_ClientToUdpPorts_t[client_id] = client_udp_port;
    }

    // Send a response
    sf::Uint16 server_udp_port;
    {
        std::lock_guard<std::mutex> m_UdpSocket_guard(m_UdpSocket_lock);
        server_udp_port = m_UdpSocket_t->getLocalPort();
    }
    sf::Packet response_packet;
    sf::Uint32 register_cmd = (sf::Uint32)ReliableCommandType::register_client;
    ReliableCommand reliable_command = {client_id, register_cmd, (sf::Uint32)m_Tick_ta};
    if (response_packet << reliable_command << server_udp_port) {
        client.send(response_packet);
        EventController::getInstance().queueEvent(
            std::shared_ptr<ClientConnectedEvent>(new ClientConnectedEvent(client_id)));
        std::cout << "Received client registration. Issued client ID " << client_id << std::endl;
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

/* UnreliableServerSend thread methods */

void NetworkingServer::broadcastGameState() {
    while (!m_StopThreads_ta) {
        sendGameState();
        std::this_thread::sleep_for(SERVER_BROADCAST_GAME_STATE_SLEEP);
    }
}

void NetworkingServer::sendGameState() {
    GameState game_state;
    {
        std::lock_guard<std::mutex> m_GameState_guard(m_GameState_lock);
        game_state = m_GameState_t;
    }
    sf::Packet packet = pack_game_state(game_state);
    {
        std::lock_guard<std::mutex> m_UdpSocket_guard(m_UdpSocket_lock);
        std::lock_guard<std::mutex> m_ClientToUdpPorts_guard(m_ClientToUdpPorts_lock);

        for (auto& it : m_ClientToUdpPorts_t) {
            sf::Uint16& client_udp_port = it.second;
            sf::Socket::Status status = sf::Socket::Partial;
            while (status == sf::Socket::Partial) {
                status = m_UdpSocket_t->send(packet, CLIENT_IP, client_udp_port);
            }
        }
    }
}
