#include "NetworkingServer.hpp"
#include <utility>
#include <vector>
#include <memory>
#include "../common/network_util.hpp"
#include "../common/game_state.hpp"

NetworkingServer::NetworkingServer():mCurrTick(0) {
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

void NetworkingServer::handleRealtimeCommand(
    sf::Socket::Status status,
    sf::Packet command_packet,
    sf::UdpSocket& rt_server,
    sf::IpAddress& sender,
    unsigned short port
) {
    RealtimeCommand realtime_command;
    command_packet >> realtime_command;
    switch (realtime_command.command) {
        case (sf::Uint32)RealtimeCommandType::move: {
            move(command_packet, realtime_command.client_id, realtime_command.tick);
            break;
        }
        case (sf::Uint32)RealtimeCommandType::fetch_state: {
            // sample current state every 100 ms, this simply packages and returns it
            GameState game_state = {
                static_cast<sf::Uint32>(mCurrTick), mGroupUpdates.copy(), mMineUpdates.copy()};
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

void NetworkingServer::move(sf::Packet command_packet, sf::Uint32 client_id, sf::Uint32 tick) {
    sf::Vector2f direction;
    if (command_packet >> direction) {
        int drift = std::abs(static_cast<int>((mCurrTick - tick)));
        if (drift < CMD_DRIFT_THRESHOLD) {
            // your newest commands will overwrite old ones
            mClientMoves.set(client_id, direction);
        } else {
            std::cout
                << "Receive move command with tick drifted past drift threshold. "
                << "Drift value is: "
                << drift
                << std::endl;
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

void NetworkingServer::handleApiCommand(
    sf::Socket::Status status,
    sf::Packet command_packet,
    sf::SocketSelector& selector,
    sf::TcpSocket& client,
    std::list<sf::TcpSocket*>& clients
) {
    sf::Uint32 api_command_type;
    switch (status) {
        case sf::Socket::Done:
            if (command_packet >> api_command_type) {
                if (api_command_type == (sf::Uint32)APICommandType::register_client) {
                    registerClient(client);
                } else if (api_command_type == (sf::Uint32)APICommandType::toggle_groupable) {
                    updateGroupable(client);
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
    clients.remove(client);
    mClientGroupable.erase(mClientSocketsToIds.get(client));
    mClientMoves.erase(mClientSocketsToIds.get(client));
    mClientSocketsToIds.erase(client);
}

void NetworkingServer::updateGroupable(sf::TcpSocket& client) {
    sf::Packet response_packet;
    sf::Uint32 toggle_groupable_cmd = (sf::Uint32)APICommandType::toggle_groupable;
    sf::Uint32 client_id = mClientSocketsToIds.get(&client);
    ApiCommand api_command = {
      client_id,
      toggle_groupable_cmd,
      (sf::Uint32) mCurrTick
    };
    if (response_packet << api_command) {
        client.send(response_packet);
    }
    if (!mClientGroupable.has_key(client_id)) {
        mClientGroupable.set(client_id, true);
    } else {
        bool groupable = 1 ^ mClientGroupable.get(client_id);
        mClientGroupable.set(client_id, groupable);
    }
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
        mClientSocketsToIds.set(&client, mClientIdCounter);
        mClientIdCounter++;
    }
}


// Main Thread Methods

client_inputs NetworkingServer::collectClientInputs() {
    // Give clients a window to write inputs
    mClientSocketsToIds.unlock();
    mClientMoves.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mClientSocketsToIds.lock();
    mClientMoves.lock();

    // Get client inputs
    std::vector<int> client_ids = getClientIds();
    std::vector<client_direction_update> client_direction_updates = getClientDirectionUpdates();
    std::vector<client_groupability_update> client_groupability_updates =
        getClientGroupabilityUpdates();

    client_inputs cis = {client_ids, client_direction_updates, client_groupability_updates};
    return cis;
}

std::vector<int> NetworkingServer::getClientIds() {
    std::vector<int> client_ids;
    auto client_sockets_to_ids = mClientSocketsToIds.forceCopy();
    std::transform(
        client_sockets_to_ids.begin(), client_sockets_to_ids.end(), std::back_inserter(client_ids),
        [](std::pair<sf::TcpSocket*, sf::Int32> pair){return pair.second;});
    return client_ids;
}

std::vector<client_direction_update> NetworkingServer::getClientDirectionUpdates() {
    std::vector<client_direction_update> client_direction_updates;
    for (const auto& client_move : mClientMoves.forceCopy()) {
        client_direction_update cd = {
            client_move.first, client_move.second.x, client_move.second.y};
        client_direction_updates.push_back(cd);
    }

    return client_direction_updates;
}

std::vector<client_groupability_update> NetworkingServer::getClientGroupabilityUpdates() {
    std::vector<client_groupability_update> client_groupability_updates;
    for (const auto& client_groupable : mClientGroupable.forceCopy()) {
        client_groupability_update cgu = {
            client_groupable.first, client_groupable.second};
        client_groupability_updates.push_back(cgu);
    }

    return client_groupability_updates;
}

void NetworkingServer::setState(
    std::vector<std::shared_ptr<Group>> active_groups,
    std::vector<std::shared_ptr<Mine>> active_mines) {
    mGroupUpdates.clear();
    for (const auto active_group : active_groups) {
        sf::Uint32 group_id = active_group->getId();
        sf::Vector2f position = active_group->getCircle()->getPosition();
        bool groupable = active_group->getGroupable();
        float radius = active_group->getCircle()->getRadius();
        GroupUpdate group_update = {group_id, position.x, position.y, radius, groupable};
        mGroupUpdates.add(group_update);
    }

    mMineUpdates.clear();
    for (const auto active_mine : active_mines) {
        sf::Uint32 mine_id = active_mine->getId();
        sf::Vector2f position = active_mine->getCircle()->getPosition();
        float radius = active_mine->getCircle()->getRadius();
        MineUpdate mine_update = {mine_id, position.x, position.y, radius};
        mMineUpdates.add(mine_update);
    }
}

void NetworkingServer::incrementTick() {
    mCurrTick++;
}
