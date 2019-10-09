#include "NetworkingServer.hpp"
#include <utility>
#include <vector>
#include <memory>
#include <atomic>

#include "../../common/util/network_util.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/events/ClientConnectedEvent.hpp"
#include "../../common/events/ClientDisconnectedEvent.hpp"
#include "../../common/util/game_settings.hpp"


sf::Time TCP_TIMEOUT = sf::milliseconds(100);;

NetworkingServer::NetworkingServer():mGameState_t() {
    std::cout << "Starting ungroup game server." << std::endl;;

    createUdpSocket();

    mReliableRecvSend = std::thread(&NetworkingServer::reliableRecvSend, this);
    mUnreliableRecv = std::thread(&NetworkingServer::unreliableRecv, this);
    mBroadcastGameStateThread = std::thread(&NetworkingServer::broadcastGameState, this);
}

NetworkingServer::~NetworkingServer() {
    std::cout << "Deconsturcting NetworkingServer" << std::endl;

    std::cout << "Joining threads..." << std::endl;
    mStopThreads_ta = true;
    mReliableRecvSend.join();
    mUnreliableRecv.join();
    mBroadcastGameStateThread.join();

    std::cout << "Deconstructed NetworkingServer." << std::endl;
}

/* Main thread methods */


void NetworkingServer::createUdpSocket() {
    std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
    mUdpSocket_t = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket);
    mUdpSocket_t->bind(sf::Socket::AnyPort);
    mUdpSocket_t->setBlocking(false);
}

ClientInputs NetworkingServer::collectClientInputs() {
    // Give clients a window to write inputs
    {
        mClientReliableUpdates_lock.unlock();
        mClientUnreliableUpdates_lock.unlock();
        mGameState_lock.unlock();
        EventController::getInstance().unlock();
        std::this_thread::sleep_for(SERVER_INPUT_WINDOW_SLEEP);
        mClientReliableUpdates_lock.lock();
        mClientUnreliableUpdates_lock.lock();
        mGameState_lock.lock();
        EventController::getInstance().lock();
    }

    // Get client inputs
    ClientInputs cis = {
        mClientUnreliableUpdates_t,
        mClientReliableUpdates_t,
    };

    mClientUnreliableUpdates_t.clear();
    mClientReliableUpdates_t.clear();

    return cis;
}

void NetworkingServer::setClientToPlayerId(sf::Int32 client_id, int player_id) {
    mClientToPlayerIds_t[client_id] = player_id;
}

void NetworkingServer::setState(const GameState& gs) {
    mGameState_t = gs;
}

void NetworkingServer::incrementTick() {
    mTick_ta++;
}

unsigned int NetworkingServer::getTick() const {
    return static_cast<unsigned int>(mTick_ta);
}

void NetworkingServer::setTick(unsigned int tick) {
    mTick_ta = tick;
}

/* UnreliableServer thread methods */

void NetworkingServer::unreliableRecv() {
    while (!mStopThreads_ta) {
        sf::IpAddress sender;
        unsigned short port;
        sf::Packet command_packet;
        sf::Socket::Status status;

        {
            std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
            status = mUdpSocket_t->receive(command_packet, sender, port);
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
        case (sf::Uint32) UnreliableCommandType::client_unreliable_update: {
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
    int drift = std::abs(static_cast<int>((mTick_ta - client_tick)));
    if (drift < CMD_DRIFT_THRESHOLD) {
        ClientUnreliableUpdate client_unreliable_update;
        packet >> client_unreliable_update;
        ClientIdAndUnreliableUpdate client_id_and_unreliable_update = {client_id,
            client_unreliable_update};
        {
            std::lock_guard<std::mutex> mClientUnreliableUpdates_guard(
                mClientUnreliableUpdates_lock);
            mClientUnreliableUpdates_t.push_back(client_id_and_unreliable_update);
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
    while (!mStopThreads_ta) {
        // Make the selector wait for data on any socket
        if (selector.wait(TCP_TIMEOUT)) {
            // Test the listener
            if (selector.isReady(listener)) {
                // The listener is ready: there is a pending connection
                auto socket = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket());
                if (listener.accept(*socket) == sf::Socket::Done) {
                    selector.add(*socket);
                    mClients.push_back(std::make_pair(mClientIdCounter++, std::move(socket)));
                } else {
                    socket.reset();
                }
            } else {
                // The listener socket is not ready, test all other sockets (the clients)
                for (auto &client : mClients) {
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
  sf::SocketSelector& selector, sf::TcpSocket& socket, sf::Uint32 client_id) {
    sf::Uint32 reliable_command_type;
    switch (status) {
        case sf::Socket::Done:
            if (command_packet >> reliable_command_type) {
                if (reliable_command_type == (sf::Uint32) ReliableCommandType::register_client) {
                    registerClient(command_packet, socket, client_id);
                } else if (reliable_command_type == (sf::Uint32) ReliableCommandType::player_id) {
                    sendPlayerId(socket, client_id);
                } else if (reliable_command_type == \
                    (sf::Uint32) ReliableCommandType::client_reliable_update) {
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
    // TODO(sourenp): Remove client from mClients
    EventController::getInstance().queueEvent(
        std::shared_ptr<ClientDisconnectedEvent>(new ClientDisconnectedEvent(client_id)));
}

void NetworkingServer::sendPlayerId(sf::TcpSocket& socket, sf::Uint32 client_id) {
    sf::Uint32 player_id;
    {
        std::lock_guard<std::mutex> mClientToPlayerIds_guard(mClientToPlayerIds_lock);
        if (mClientToPlayerIds_t.count(client_id) == 0) {
            return;
        }
        player_id = mClientToPlayerIds_t[client_id];
    }
    sf::Packet packet;
    sf::Uint32 player_id_cmd = ReliableCommandType::player_id;
    ReliableCommand reliable_command = {
        client_id,
        player_id_cmd,
        mTick_ta
    };
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
        std::lock_guard<std::mutex> mClientReliableUpdates_guard(mClientReliableUpdates_lock);
        mClientReliableUpdates_t.push_back(client_id_and_reliable_update);
    }
}

void NetworkingServer::registerClient(sf::Packet packet, sf::TcpSocket& client,
    sf::Uint32 client_id) {
    // Save client udp port
    sf::Uint16 client_udp_port;
    packet >> client_udp_port;
    {
        std::lock_guard<std::mutex> mClientToUdpPorts_guard(mClientToUdpPorts_lock);
        mClientToUdpPorts_t[client_id] = client_udp_port;
    }

    // Send a response
    sf::Uint16 server_udp_port;
    {
        std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
        server_udp_port = mUdpSocket_t->getLocalPort();
    }
    sf::Packet response_packet;
    sf::Uint32 register_cmd = (sf::Uint32)ReliableCommandType::register_client;
    ReliableCommand reliable_command = {client_id, register_cmd, (sf::Uint32) mTick_ta};
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
    while (!mStopThreads_ta) {
        sendGameState();
        std::this_thread::sleep_for(SERVER_BROADCAST_GAME_STATE_SLEEP);
    }
}

void NetworkingServer::sendGameState() {
    GameState game_state;
    {
        std::lock_guard<std::mutex> mGameState_guard(mGameState_lock);
        game_state = mGameState_t;
    }
    sf::Packet packet = pack_game_state(game_state);
    {
        std::lock_guard<std::mutex> mUdpSocket_guard(mUdpSocket_lock);
        std::lock_guard<std::mutex> mClientToUdpPorts_guard(mClientToUdpPorts_lock);

        for (auto& it: mClientToUdpPorts_t) {
            sf::Uint16& client_udp_port = it.second;
            sf::Socket::Status status = sf::Socket::Partial;
            while (status == sf::Socket::Partial) {
                status = mUdpSocket_t->send(packet, CLIENT_IP, client_udp_port);
            }
        }
    }
}