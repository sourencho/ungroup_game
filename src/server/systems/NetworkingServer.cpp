#include "NetworkingServer.hpp"
#include <utility>
#include <vector>
#include <memory>

#include "../../common/util/network_util.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/events/ClientConnectedEvent.hpp"
#include "../../common/events/ClientDisconnectedEvent.hpp"


int INPUT_WINDOW_SLEEP = 200;
sf::Time TCP_TIMEOUT = sf::milliseconds(100);;


NetworkingServer::NetworkingServer():mGameState(), mTick(0), mStopThreads(false) {
    std::cout << "Starting ungroup game server." << std::endl;;
    mReliableThread = std::thread(&NetworkingServer::reliableServer, this);
    mUnreliableThread = std::thread(&NetworkingServer::unreliableServer, this);
}

NetworkingServer::~NetworkingServer() {
    std::cout << "Deconsturcting NetworkingServer" << std::endl;
    mStopThreads = true;
    std::cout << "Joining unreliable thread..." << std::endl;
    mUnreliableThread.join();
    std::cout << "Joining reliable thread..." << std::endl;
    mReliableThread.join();
    std::cout << "Done." << std::endl;
}

// UnreliableServer Thread Methods

void NetworkingServer::unreliableServer() {
    sf::UdpSocket udp_socket;
    udp_socket.setBlocking(false);
    udp_socket.bind(4888);
    sf::Packet command_packet;

    while (!mStopThreads) {
        sf::IpAddress sender;
        unsigned short port;
        sf::Socket::Status status = udp_socket.receive(command_packet, sender, port);
        handleUnreliableCommand(status, command_packet, udp_socket, sender, port);
    }
}

void NetworkingServer::handleUnreliableCommand(sf::Socket::Status status, sf::Packet command_packet,
  sf::UdpSocket& udp_socket, sf::IpAddress& sender, unsigned short port) {
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
        case (sf::Uint32)UnreliableCommandType::fetch_state: {
            sendState(udp_socket, sender, port);
            break;
        }
        default: {
            std::cout
                << "Unknown command code sent: "
                << unreliable_command.command
                << std::endl;
            break;
        }
    }
}

void NetworkingServer::sendState(sf::UdpSocket& udp_socket, sf::IpAddress& sender,
  unsigned short port) {
    // sample current state every 100 ms, this simply packages and returns it
    GameState game_state = mGameState.get();
    sf::Packet game_state_packet = pack_game_state(game_state);
    auto status = sf::Socket::Partial;
    while (status == sf::Socket::Partial) {
        status = udp_socket.send(game_state_packet, sender, port);
    }
}

void NetworkingServer::setClientUnreliableUpdate(sf::Packet packet, int client_id,
  unsigned int client_tick) {
    int drift = std::abs(static_cast<int>((mTick - client_tick)));
    if (drift < CMD_DRIFT_THRESHOLD) {
        ClientUnreliableUpdate client_unreliable_update;
        packet >> client_unreliable_update;
        ClientIdAndUnreliableUpdate client_id_and_unreliable_update = {client_id,
            client_unreliable_update};
        mClientIdAndUnreliableUpdates.add(client_id_and_unreliable_update);
    } else {
        std::cout
            << "Receive client_update command with tick drifted past drift threshold. "
            << "Drift value is: "
            << drift
            << std::endl;
    }
}

// ReliableServer Thread Methods

void NetworkingServer::reliableServer() {
    // Create a socket to listen to new connections
    sf::TcpListener listener;
    // Reliable socket
    listener.listen(4844);

    // Create a selector
    sf::SocketSelector selector;

    // Add the listener to the selector
    selector.add(listener);

    // Endless loop that waits for new connections
    while (!mStopThreads) {
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
    }
}

void NetworkingServer::handleReliableCommand(sf::Socket::Status status, sf::Packet command_packet,
  sf::SocketSelector& selector, sf::TcpSocket& socket, sf::Uint32 client_id) {
    sf::Uint32 reliable_command_type;
    switch (status) {
        case sf::Socket::Done:
            if (command_packet >> reliable_command_type) {
                if (reliable_command_type == (sf::Uint32) ReliableCommandType::register_client) {
                    registerClient(socket, client_id);
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
            std::cout << "Removing client due to TCP error."
                << std::endl;
            clientDisconnect(socket, client_id);
            selector.remove(socket);
            break;
        default:
            std::cout << "TCP client sent unkown signal." << std::endl;
            break;
    }
}

void NetworkingServer::clientDisconnect(sf::TcpSocket& client, sf::Uint32 client_id) {
    EventController::getInstance().queueEvent(
        std::shared_ptr<ClientDisconnectedEvent>(new ClientDisconnectedEvent(client_id)));
}

void NetworkingServer::sendPlayerId(sf::TcpSocket& socket, sf::Uint32 client_id) {
    if (!mClientToPlayerIds.has_key(client_id)) {
        return;
    }
    sf::Packet packet;
    int player_id_cmd = ReliableCommandType::player_id;
    PlayerId pi = {(sf::Uint32) mClientToPlayerIds.get(client_id)};
    ReliableCommand reliable_command = {
        (sf::Uint32) client_id,
        (sf::Uint32) player_id_cmd,
        (sf::Uint32) mTick
    };
    if (packet << reliable_command << pi) {
        socket.send(packet);
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}

void NetworkingServer::setClientReliableUpdate(sf::Packet packet, int client_id) {
    ClientReliableUpdate client_reliable_update;
    packet >> client_reliable_update;
    ClientIdAndReliableUpdate client_id_and_reliable_update = {client_id, client_reliable_update};
    mClientIdAndReliableUpdates.add(client_id_and_reliable_update);
}

void NetworkingServer::registerClient(sf::TcpSocket& client, sf::Uint32 client_id) {
    sf::Packet response_packet;
    sf::Uint32 register_cmd = (sf::Uint32)ReliableCommandType::register_client;
    ReliableCommand reliable_command = {client_id, register_cmd, (sf::Uint32) mTick};
    if (response_packet << reliable_command) {
        client.send(response_packet);
        std::cout << "Received client registration. Issued client ID " << client_id << std::endl;
        EventController::getInstance().queueEvent(
            std::shared_ptr<ClientConnectedEvent>(new ClientConnectedEvent(client_id)));
    } else {
        std::cout << "Failed to form packet" << std::endl;
    }
}


// Main Thread Methods

ClientInputs NetworkingServer::collectClientInputs() {
    // Give clients a window to write inputs
    EventController::getInstance().unlock();
    mClientIdAndUnreliableUpdates.unlock();
    mClientIdAndReliableUpdates.unlock();
    mGameState.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(INPUT_WINDOW_SLEEP));
    mClientIdAndUnreliableUpdates.lock();
    mClientIdAndReliableUpdates.lock();
    mGameState.lock();
    EventController::getInstance().lock();

    // Get client inputs
    ClientInputs cis = {
        mClientIdAndUnreliableUpdates.forceGetAndClear(),
        mClientIdAndReliableUpdates.forceGetAndClear()};
    return cis;
}

void NetworkingServer::setClientToPlayerId(sf::Int32 client_id, int player_id) {
    mClientToPlayerIds.forceSet(client_id, player_id);
}

void NetworkingServer::setState(GameState gs) {
    mGameState.forceSet(gs);
}

void NetworkingServer::incrementTick() {
    mTick++;
}

unsigned int NetworkingServer::getTick() const {
    return static_cast<unsigned int>(mTick);
}

void NetworkingServer::setTick(unsigned int tick) {
    mTick = tick;
}
