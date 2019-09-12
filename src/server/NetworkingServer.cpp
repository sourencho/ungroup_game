#include "NetworkingServer.hpp"
#include <utility>
#include <vector>
#include <memory>

#include "../common/network_util.hpp"
#include "../common/events/EventController.hpp"
#include "../common/events/ClientConnectedEvent.hpp"
#include "../common/events/ClientDisconnectedEvent.hpp"


int INPUT_WINDOW_SLEEP = 100;


NetworkingServer::NetworkingServer():
  mCurrTick(0) {
    std::cout << "Starting ungroup game server." << std::endl;;
    std::thread reliable_thread(&NetworkingServer::reliableServer, this);
    std::thread unreliable_thread(&NetworkingServer::unreliableServer, this);
    reliable_thread.detach();
    unreliable_thread.detach();
}

NetworkingServer::~NetworkingServer() {}


// UnreliableServer Thread Methods

void NetworkingServer::unreliableServer() {
    sf::UdpSocket rt_server;
    rt_server.bind(4888);
    sf::Packet command_packet;

    while (true) {
        sf::IpAddress sender;
        unsigned short port;
        sf::Socket::Status status = rt_server.receive(command_packet, sender, port);
        handleUnreliableCommand(status, command_packet, rt_server, sender, port);
    }
}

void NetworkingServer::handleUnreliableCommand(sf::Socket::Status status, sf::Packet command_packet,
  sf::UdpSocket& rt_server, sf::IpAddress& sender, unsigned short port) {
    UnreliableCommand unreliable_command;
    command_packet >> unreliable_command;
    switch (unreliable_command.command) {
        case (sf::Uint32) UnreliableCommandType::client_unreliable_update: {
            setClientUnreliableUpdate(command_packet, unreliable_command.client_id,
                unreliable_command.tick);
            break;
        }
        case (sf::Uint32)UnreliableCommandType::fetch_state: {
            sendState(rt_server, sender, port);
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

void NetworkingServer::sendState(sf::UdpSocket& rt_server, sf::IpAddress& sender,
  unsigned short port) {
    // sample current state every 100 ms, this simply packages and returns it
    GameState game_state = mGameState.get();
    sf::Packet game_state_packet = pack_game_state(game_state);
    rt_server.send(game_state_packet, sender, port);
}

void NetworkingServer::setClientUnreliableUpdate(sf::Packet packet, int client_id,
  int client_tick) {
    int drift = std::abs(static_cast<int>((mCurrTick - client_tick)));
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

    // Create a vector to store the future clients
    std::list<sf::TcpSocket*> clients;

    // Add the listener to the selector
    selector.add(listener);

    // Endless loop that waits for new connections
    while (true) {
        // Make the selector wait for data on any socket
        if (selector.wait()) {
            // Test the listener
            if (selector.isReady(listener)) {
                // The listener is ready: there is a pending connection
                sf::TcpSocket* client = new sf::TcpSocket;
                if (listener.accept(*client) == sf::Socket::Done) {
                    // Add the new client to the clients list
                    mClients.push_back(client);
                    // Add the new client to the selector so that we will
                    // be notified when he sends something
                    selector.add(*client);
                } else {
                    // Error, we won't get a new connection, delete the socket
                    delete client;
                }
            } else {
                // The listener socket is not ready, test all other sockets (the clients)
                for (auto it = mClients.begin(); it != mClients.end(); ++it) {
                    sf::TcpSocket& client = **it;
                    if (selector.isReady(client)) {
                        // The client has sent some data, we can receive it
                        sf::Packet command_packet;
                        sf::Socket::Status status = client.receive(command_packet);
                        handleReliableCommand(status, command_packet, selector, client, clients);
                    }
                }
            }
        }
    }
}

void NetworkingServer::handleReliableCommand(sf::Socket::Status status, sf::Packet command_packet,
  sf::SocketSelector& selector,
  sf::TcpSocket& client,
  std::list<sf::TcpSocket*>& clients) {
    sf::Uint32 reliable_command_type;
    switch (status) {
        case sf::Socket::Done:
            if (command_packet >> reliable_command_type) {
                if (reliable_command_type == (sf::Uint32) ReliableCommandType::register_client) {
                    registerClient(client);
                } else if (reliable_command_type == (sf::Uint32) ReliableCommandType::player_id) {
                    sendPlayerId(client);
                } else if (reliable_command_type == \
                    (sf::Uint32) ReliableCommandType::client_reliable_update) {
                    setClientReliableUpdate(command_packet, mClientSocketsToIds.get(&client));
                }
            }
            break;
        case sf::TcpSocket::Error:
            std::cout << "TCP client encountered error. Removing client." << std::endl;
            selector.remove(client);
            deleteClient(&client, clients);
            break;
        case sf::TcpSocket::Disconnected:
            std::cout << "TCP client disconnected. Removing client. " << std::endl;
            selector.remove(client);
            deleteClient(&client, clients);
            break;
        default:
            std::cout << "TCP client sent unkown signal." << std::endl;
            break;
    }
}

void NetworkingServer::deleteClient(sf::TcpSocket* client, std::list<sf::TcpSocket*> clients) {
    int deleted_client_id = mClientSocketsToIds.get(client);
    clients.remove(client);
    mClientSocketsToIds.erase(client);
    EventController::getInstance().queueEvent(
        std::shared_ptr<ClientDisconnectedEvent>(new ClientDisconnectedEvent(deleted_client_id)));
}

void NetworkingServer::sendPlayerId(sf::TcpSocket& client) {
    int client_id = mClientSocketsToIds.get(&client);
    if (!mClientToPlayerIds.has_key(client_id)) {
        return;
    }
    sf::Packet packet;
    int player_id_cmd = ReliableCommandType::player_id;
    PlayerId pi = {(sf::Uint32) mClientToPlayerIds.get(client_id)};
    ReliableCommand reliable_command = {
        (sf::Uint32) client_id,
        (sf::Uint32) player_id_cmd,
        (sf::Uint32) mCurrTick
    };
    if (packet << reliable_command << pi) {
        client.send(packet);
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

void NetworkingServer::registerClient(sf::TcpSocket& client) {
    sf::Packet response_packet;
    sf::Uint32 register_cmd = (sf::Uint32)ReliableCommandType::register_client;
    ReliableCommand reliable_command = {mClientIdCounter, register_cmd, (sf::Uint32) mCurrTick};
    if (response_packet << reliable_command) {
        client.send(response_packet);
        std::cout
            << "Received client registration. Issued client ID "
            << mClientIdCounter
            << std::endl;
        int new_client_id = mClientIdCounter++;
        mClientSocketsToIds.set(&client, new_client_id);

        EventController::getInstance().queueEvent(
            std::shared_ptr<ClientConnectedEvent>(new ClientConnectedEvent(new_client_id)));
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

void NetworkingServer::setState(std::vector<std::shared_ptr<Group>> groups,
  std::vector<std::shared_ptr<Mine>> mines) {
    std::vector<GroupUpdate> group_updates;
    std::vector<MineUpdate> mine_updates;
    GameState gs = {mCurrTick, group_updates, mine_updates};
    std::transform(
        groups.begin(), groups.end(), std::back_inserter(gs.group_updates),
        [](std::shared_ptr<Group> group){return group->getUpdate();});

    std::transform(
        mines.begin(), mines.end(), std::back_inserter(gs.mine_updates),
        [](std::shared_ptr<Mine> mine){return mine->getUpdate();});

    mGameState.forceSet(gs);
}

void NetworkingServer::incrementTick() {
    mCurrTick++;
}
