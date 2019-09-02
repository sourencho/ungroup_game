#include "NetworkingServer.hpp"
#include <utility>
#include <vector>
#include <memory>

#include "../common/network_util.hpp"

NetworkingServer::NetworkingServer():
  mCurrTick(0) {
    std::cout << "Starting ungroup game server." << std::endl;;
    std::thread api_thread(&NetworkingServer::apiServer, this);
    std::thread realtime_thread(&NetworkingServer::realtimeServer, this);
    api_thread.detach();
    realtime_thread.detach();
}

NetworkingServer::~NetworkingServer() {}


// RealtimeServer Thread Methods

void NetworkingServer::realtimeServer() {
    sf::UdpSocket rt_server;
    rt_server.bind(4888);
    sf::Packet command_packet;

    while (true) {
        sf::IpAddress sender;
        unsigned short port;
        sf::Socket::Status status = rt_server.receive(command_packet, sender, port);
        handleRealtimeCommand(status, command_packet, rt_server, sender, port);
    }
}

void NetworkingServer::handleRealtimeCommand(sf::Socket::Status status, sf::Packet command_packet,
  sf::UdpSocket& rt_server,
  sf::IpAddress& sender,
  unsigned short port) {
    ClientUpdate client_update;
    RealtimeCommand realtime_command;
    command_packet >> realtime_command;
    switch (realtime_command.command) {
        case (sf::Uint32) RealtimeCommandType::client_update: {
            int drift = std::abs(static_cast<int>((mCurrTick - realtime_command.tick)));
            if (drift < CMD_DRIFT_THRESHOLD) {
                command_packet >> client_update;
                mPlayerUpdates.add(client_update.player_update);
            } else {
                std::cout
                    << "Receive client_update command with tick drifted past drift threshold. "
                    << "Drift value is: "
                    << drift
                    << std::endl;
            }
            break;
        }
        case (sf::Uint32)RealtimeCommandType::fetch_state: {
            // sample current state every 100 ms, this simply packages and returns it
            GameState game_state = mGameState.get();
            sf::Packet game_state_packet = pack_game_state(game_state);
            rt_server.send(game_state_packet, sender, port);
            break;
        }
        default: {
            std::cout
                << "Unknown command code sent: "
                << realtime_command.command
                << std::endl;
            break;
        }
    }
}

// ApiServer Thread Methods

void NetworkingServer::apiServer() {
    // Create a socket to listen to new connections
    sf::TcpListener listener;
    // API socket
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
                        handleApiCommand(status, command_packet, selector, client, clients);
                    }
                }
            }
        }
    }
}

void NetworkingServer::handleApiCommand(sf::Socket::Status status, sf::Packet command_packet,
  sf::SocketSelector& selector,
  sf::TcpSocket& client,
  std::list<sf::TcpSocket*>& clients) {
    sf::Uint32 api_command_type;
    switch (status) {
        case sf::Socket::Done:
            if (command_packet >> api_command_type) {
                if (api_command_type == (sf::Uint32) APICommandType::register_client) {
                    registerClient(client);
                } else if (api_command_type == (sf::Uint32) APICommandType::player_id) {
                    sendPlayerId(client);
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
    int client_id = mClientSocketsToIds.get(client);
    clients.remove(client);
    mClientSocketsToIds.erase(client);
    mRemovedClientIds.add(client_id);
}

void NetworkingServer::sendPlayerId(sf::TcpSocket& client) {
    int client_id = mClientSocketsToIds.get(&client);
    if (!mClientToPlayerIds.has_key(client_id)) {
        return;
    }
    sf::Packet packet;
    int player_id_cmd = APICommandType::player_id;
    PlayerId pi = {(sf::Uint32) mClientToPlayerIds.get(client_id)};
    ApiCommand api_command = {
        (sf::Uint32) client_id,
        (sf::Uint32) player_id_cmd,
        (sf::Uint32) mCurrTick
    };
    packet << api_command << pi;
    client.send(packet);
}

void NetworkingServer::registerClient(sf::TcpSocket& client) {
    sf::Packet response_packet;
    sf::Uint32 register_cmd = (sf::Uint32)APICommandType::register_client;
    ApiCommand api_command = {mClientIdCounter, register_cmd, (sf::Uint32) mCurrTick};
    if (response_packet << api_command) {
        client.send(response_packet);
        std::cout
            << "Received client registration. Issued client ID "
            << mClientIdCounter
            << std::endl;
        int new_client_id = mClientIdCounter++;
        mClientSocketsToIds.set(&client, new_client_id);
        mNewClientIds.add(new_client_id);
    }
}


// Main Thread Methods

ClientInputs NetworkingServer::collectClientInputs() {
    // Give clients a window to write inputs
    mClientSocketsToIds.unlock();
    mNewClientIds.unlock();
    mRemovedClientIds.unlock();
    mPlayerUpdates.unlock();
    mGameState.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mClientSocketsToIds.lock();
    mNewClientIds.lock();
    mRemovedClientIds.lock();
    mPlayerUpdates.lock();
    mGameState.lock();

    // Get client inputs
    ClientInputs cis = {
        popNewClientIds(), popRemovedClientIds(), popPlayerUpdates()};
    return cis;
}

std::vector<int> NetworkingServer::popNewClientIds() {
    std::vector<int> new_client_ids = mNewClientIds.forceCopy();
    mNewClientIds.forceClear();
    return new_client_ids;
}

std::vector<int> NetworkingServer::popRemovedClientIds() {
    std::vector<int> removed_client_ids = mRemovedClientIds.forceCopy();
    mRemovedClientIds.forceClear();
    return removed_client_ids;
}

std::vector<PlayerUpdate> NetworkingServer::popPlayerUpdates() {
    std::vector<PlayerUpdate> player_updates = mPlayerUpdates.forceCopy();
    mPlayerUpdates.forceClear();
    return player_updates;
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
