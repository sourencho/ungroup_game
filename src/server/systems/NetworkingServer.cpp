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

sf::Time TCP_TIMEOUT = sf::milliseconds(300);

NetworkingServer::NetworkingServer() : m_gameState_t() {
    m_stateUdpSocketPort = createStateUdpSocket();
    m_inputUdpSocketPort = createInputUdpSocket();

    addEventListeners();

    m_reliableRecvSend = std::thread(&NetworkingServer::reliableRecvSend, this);
    m_unreliableRecv = std::thread(&NetworkingServer::unreliableRecv, this);
    m_broadcastGameStateThread = std::thread(&NetworkingServer::broadcastGameState, this);
    m_natRecvThread = std::thread(&NetworkingServer::natRecv, this);
}

NetworkingServer::~NetworkingServer() {
    m_playerReliableUpdates_lock.unlock();
    m_playerUnreliableUpdates_lock.unlock();
    m_gameState_lock.unlock();
    EventController::getInstance().unlock();

    {
        std::lock_guard<std::mutex> m_stateUdpSocket_guard(m_stateUdpSocket_lock);
        m_stateUdpSocket_t->unbind();
    }

    // disconnect any connected clients
    for (auto& client : m_clients) {
        sf::Uint32 client_id = client.first;
        sf::TcpSocket& socket = *client.second;
        socket.disconnect();
    }
    m_stopThreads_ta = true;

    m_reliableRecvSend.join();
    m_unreliableRecv.join();
    m_broadcastGameStateThread.join();
    m_natRecvThread.join();
}

/* Main thread methods */

void NetworkingServer::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_PLAYER_CREATED,
        std::bind(&NetworkingServer::handlePlayerCreatedEvent, this, std::placeholders::_1));
}

sf::Uint16 NetworkingServer::createStateUdpSocket() {
    std::lock_guard<std::mutex> m_stateUdpSocket_guard(m_stateUdpSocket_lock);
    m_stateUdpSocket_t = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    m_stateUdpSocket_t->bind(GAME_SETTINGS.SERVER_STATE_UDP_PORT);
    m_stateUdpSocket_t->setBlocking(false);
    return static_cast<sf::Uint16>(m_stateUdpSocket_t->getLocalPort());
}

sf::Uint16 NetworkingServer::createInputUdpSocket() {
    m_inputUdpSocket = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    m_inputUdpSocket->bind(GAME_SETTINGS.SERVER_INPUT_UDP_PORT);
    return static_cast<sf::Uint16>(m_inputUdpSocket->getLocalPort());
}

InputDef::PlayerInputs NetworkingServer::collectClientInputs() {
    // Give clients a window to write inputs
    {
        m_playerReliableUpdates_lock.unlock();
        m_playerUnreliableUpdates_lock.unlock();
        m_gameState_lock.unlock();
        EventController::getInstance().unlock();
        std::this_thread::sleep_for(GAME_SETTINGS.SERVER_INPUT_WINDOW_SLEEP);
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
    sf::SocketSelector selector;
    selector.add(*m_inputUdpSocket);

    sf::IpAddress sender;
    unsigned short port;
    sf::Packet command_packet;
    while (!m_stopThreads_ta) {
        if (selector.wait(GAME_SETTINGS.SERVER_UNRELIABLE_RECV_TIMEOUT_SEC)) {
            if (selector.isReady(*m_inputUdpSocket)) {
                if (m_inputUdpSocket->receive(command_packet, sender, port) == sf::Socket::Done) {
                    handleUnreliableCommand(sf::Socket::Done, command_packet, sender, port);
                }
            }
        }
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
        // We overwrite the client UDP port to send to in case it's different from
        // what we receive during registration. The only time this is the case is if
        // a router is rewriting the src port field of the UDP packets flowing from the client.
        // This can happen in the case of a Symmetric NAT. If we send the reply packet to the port
        // we get during registration, the NAT won't accept our reply and will drop it.
        // Thus, this update block is necessary for functioning in Symmetric NAT territory.
        case (sf::Uint32)UnreliableCommandType::nat_punch: {
            {
                std::lock_guard<std::mutex> m_clientToUdpPorts_guard(m_clientToUdpPorts_lock);
                if (m_clientToUdpPorts_t[unreliable_command.client_id] != port) {
                    m_clientToUdpPorts_t[unreliable_command.client_id] = port;
                }
            }
            break;
        }
        case (sf::Uint32)UnreliableCommandType::unreliable_input: {
            setClientUnreliableUpdate(command_packet, unreliable_command.client_id,
                                      unreliable_command.tick);
            break;
        }
        default: {
            std::cerr << "Unknown command code sent: " << unreliable_command.command << std::endl;
            break;
        }
    }
}

void NetworkingServer::updateDriftMetric(uint32_t player_id, uint32_t drift) {
    {
        std::lock_guard<std::mutex> m_playerIdsToDriftMetric_guard(m_playerIdsToDriftMetrics_lock);
        if (m_playerIdsToDriftMetrics_t.count(player_id) == 0) {
            m_playerIdsToDriftMetrics_t[player_id] = new TemporalMetric{30, sf::seconds(0.5f)};
        }
        m_playerIdsToDriftMetrics_t[player_id]->pushCount(drift);
        m_playerIdsToDriftMetrics_t[player_id]->update();
    }
}

void NetworkingServer::updateUpdatesMetric(uint32_t player_id) {
    {
        std::lock_guard<std::mutex> m_playerIdsToUpdatesMetric_guard(
            m_playerIdsToUpdatesMetric_lock);
        if (m_playerIdsToUpdatesMetric_t.count(player_id) == 0) {
            m_playerIdsToUpdatesMetric_t[player_id] = new TemporalMetric{30, sf::seconds(0.5f)};
        }
        m_playerIdsToUpdatesMetric_t[player_id]->pushCount();
        m_playerIdsToUpdatesMetric_t[player_id]->update();
    }
}

int32_t NetworkingServer::getPlayerIdFromClientId(uint32_t client_id) {
    {
        std::lock_guard<std::mutex> m_clientToPlayerIds_guard(m_clientToPlayerIds_lock);
        if (m_clientToPlayerIds_t.count(client_id) == 0) {
            return -1;
        }
        return m_clientToPlayerIds_t[client_id];
    }
}

void NetworkingServer::setClientUnreliableUpdate(sf::Packet packet, int client_id,
                                                 uint32_t client_tick) {
    int32_t player_id = getPlayerIdFromClientId(client_id);
    // client ID was not found
    if (player_id < 0) {
        return;
    }
    uint32_t drift = std::abs(static_cast<int>((m_tick_ta - client_tick)));

    updateDriftMetric(player_id, drift);
    if (drift < CMD_DRIFT_THRESHOLD) {
        updateUpdatesMetric(player_id);

        InputDef::UnreliableInput unreliable_input;
        packet >> unreliable_input;
        InputDef::PlayerUnreliableInput player_unreliable_input = {static_cast<uint32_t>(player_id),
                                                                   unreliable_input};
        {
            std::lock_guard<std::mutex> m_playerUnreliableUpdates_guard(
                m_playerUnreliableUpdates_lock);
            m_playerUnreliableUpdates_t.push_back(player_unreliable_input);
        }
    }
}

float NetworkingServer::getBroadcastGameStateRate() {
    {
        std::lock_guard<std::mutex> m_playerIdsToUpdatesMetric_guard(
            m_playerIdsToUpdatesMetric_lock);
        return m_broadcastGameStateMetric_t.getRate(sf::seconds(1));
    }
}

std::unordered_map<sf::Uint32, float> NetworkingServer::getPlayerUnreliableUpdatesRates() {
    {
        std::lock_guard<std::mutex> m_playerIdsToUpdatesMetric_guard(
            m_playerIdsToUpdatesMetric_lock);
        for (const auto& kv : m_playerIdsToUpdatesMetric_t) {
            m_playerIdsToUpdatesRates_t[kv.first] = (kv.second)->getRate(sf::seconds(1));
        }

        return m_playerIdsToUpdatesRates_t;
    }
}

std::unordered_map<sf::Uint32, float> NetworkingServer::getPlayerTickDrifts() {
    {
        std::lock_guard<std::mutex> m_playerIdsToDriftMetrics_guard(m_playerIdsToDriftMetrics_lock);
        for (const auto& kv : m_playerIdsToDriftMetrics_t) {
            m_playerIdsToDrifts_t[kv.first] = (kv.second)->getAverage();
        }
        return m_playerIdsToDrifts_t;
    }
}

/* ReliableServer thread methods */

void NetworkingServer::reliableRecvSend() {
    // Create a socket to listen to new connections
    sf::TcpListener listener;
    // Reliable socket
    listener.listen(GAME_SETTINGS.SERVER_TCP_PORT);

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
                    int new_client_id = m_clientIdCounter++;
                    // store the IP for sending UDP replies later
                    {
                        std::lock_guard<std::mutex> m_clientToIps_guard(m_clientToIps_lock);
                        m_clientToIps_t[new_client_id] = (*socket).getRemoteAddress();
                    }
                    m_clients.push_back(std::make_pair(new_client_id, std::move(socket)));
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

        std::this_thread::sleep_for(GAME_SETTINGS.SERVER_RELIABLE_REVC_SEND_SLEEP);
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
            std::cerr << "Removing client due to TCP dsconnect/error." << std::endl;
            clientDisconnect(socket, client_id);
            selector.remove(socket);
            break;
        default:
            std::cerr << "TCP client sent unkown signal." << std::endl;
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
        std::cerr << "Failed to form packet" << std::endl;
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
    sf::Packet response_packet;
    sf::Uint32 register_cmd = (sf::Uint32)ReliableCommandType::register_client;
    ReliableCommand reliable_command = {client_id, register_cmd, (sf::Uint32)m_tick_ta};
    if (response_packet << reliable_command << m_inputUdpSocketPort << m_stateUdpSocketPort) {
        socket.send(response_packet);
        EventController::getInstance().queueEvent(
            std::shared_ptr<ClientConnectedEvent>(new ClientConnectedEvent(client_id)));
    } else {
        std::cerr << "Failed to form packet" << std::endl;
    }
}

/* broadcastGameState thread methods */

void NetworkingServer::broadcastGameState() {
    while (!m_stopThreads_ta) {
        sendGameState();
        std::this_thread::sleep_for(GAME_SETTINGS.SERVER_BROADCAST_GAME_STATE_SLEEP);
    }
}

void NetworkingServer::sendGameState() {
    GameState game_state;
    {
        std::lock_guard<std::mutex> m_gameState_guard(m_gameState_lock);
        game_state = m_gameState_t;
    }
    sf::Packet packet;
    packet << game_state;
    {
        std::lock_guard<std::mutex> m_stateUdpSocket_guard(m_stateUdpSocket_lock);
        std::lock_guard<std::mutex> m_clientToUdpPorts_guard(m_clientToUdpPorts_lock);

        for (auto& it : m_clientToUdpPorts_t) {
            sf::Uint16& client_udp_port = it.second;
            sf::Socket::Status status = sf::Socket::Partial;
            while (status == sf::Socket::Partial) {
                int client_id = it.first;
                {
                    std::lock_guard<std::mutex> m_clientToIps_guard(m_clientToIps_lock);
                    status = m_stateUdpSocket_t->send(packet, m_clientToIps_t[client_id],
                                                      client_udp_port);
                }
            }
        }
    }
    {
        std::lock_guard<std::mutex> m_playerIdsToUpdatesMetric_guard(
            m_playerIdsToUpdatesMetric_lock);
        m_broadcastGameStateMetric_t.pushCount();
        m_broadcastGameStateMetric_t.update();
    }
}

/* natRecv thread methods */

void NetworkingServer::natRecv() {
    sf::IpAddress sender;
    unsigned short port;
    sf::Packet command_packet;
    sf::Socket::Status status;
    while (!m_stopThreads_ta) {
        {
            std::lock_guard<std::mutex> m_stateUdpSocket_guard(m_stateUdpSocket_lock);
            status = m_stateUdpSocket_t->receive(command_packet, sender, port);
        }
        handleUnreliableCommand(status, command_packet, sender, port);

        std::this_thread::sleep_for(GAME_SETTINGS.SERVER_NAT_RECV_SLEEP);
    }
}
