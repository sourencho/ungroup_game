#include "NetworkingServer.hpp"

NetworkingServer::NetworkingServer():mCurrTick(0) {
    std::cout << "Starting ungroup game server.\n";
    std::thread api_thread(&NetworkingServer::ApiServer, this);
    std::thread realtime_thread(&NetworkingServer::RealtimeServer, this);
    api_thread.detach();
    realtime_thread.detach();
}

NetworkingServer::~NetworkingServer() {}


// RealtimeServer Thread Methods

void NetworkingServer::RealtimeServer() {
    sf::UdpSocket rt_server;
    rt_server.bind(4888);

    while (true) {
        sf::Uint32 ct = mCurrTick;
        sf::Packet game_state_packet;
        sf::Packet command_packet;
        sf::IpAddress sender;
        unsigned short port;
        rt_server.receive(command_packet, sender, port);

        // Extract the variables contained in the command_packet
        sf::Uint32 client_id;
        sf::Uint32 tick;
        sf::Uint32 realtime_command;
        if (command_packet >> client_id >> realtime_command >> tick) {
            switch(realtime_command) {
                case (sf::Uint32)RealtimeCommand::move:
                    Move(command_packet, client_id, tick);
                    break;
                case (sf::Uint32)RealtimeCommand::fetch_state:
                    //std::cout << "Sending game state to client: " << sender << " " << port << std::endl;
                    // sample current state every 100 ms, this simply packages and returns it
                    game_state_packet << ct; // should have error handling for <<
                    for(const auto client_group_update: mClientGroupUpdates.copy()) {
                        game_state_packet << client_group_update.client_id
                            << client_group_update.x_pos
                            << client_group_update.y_pos
                            << client_group_update.size;
                    }
                    rt_server.send(game_state_packet, sender, port);
                    break;
                default:
                    std::cout << "Unknown command code sent: " << realtime_command << std::endl;
                    break;
            }
        }
    }
}

void NetworkingServer::Move(sf::Packet command_packet, sf::Uint32 client_id, sf::Uint32 tick) {

    float x_dir;
    float y_dir;
    if (command_packet >> x_dir >> y_dir) {
        int drift = std::abs((int)((mCurrTick - tick)));
        if (drift < CMD_DRIFT_THRESHOLD) {
            // your newest commands will overwrite old ones
            direction d = {x_dir, y_dir};
            mClientMoves.set(client_id, d);
        } else {
            std::cout << "Receive move command with tick drifted past drift threshold. Drift value is: "
                << drift << std::endl;
        }
    }

}


// ApiServer Thread Methods

void NetworkingServer::ApiServer() {
    // Create a socket to listen to new connections
    sf::TcpListener listener;
    // API socket
    listener.listen(4844);

    // Create a vector to store the future clients
    std::list<sf::TcpSocket*> clients;

    // Create a selector
    sf::SocketSelector selector;

    // Add the listener to the selector
    selector.add(listener);

    // Endless loop that waits for new connections
    while (true)
    {
        // Make the selector wait for data on any socket
        if (selector.wait())
        {
            // Test the listener
            if (selector.isReady(listener))
            {
                // The listener is ready: there is a pending connection
                sf::TcpSocket* client = new sf::TcpSocket;
                if (listener.accept(*client) == sf::Socket::Done)
                {
                    // Add the new client to the clients list
                    clients.push_back(client);
                    // Add the new client to the selector so that we will
                    // be notified when he sends something
                    selector.add(*client);
                } else {
                    // Error, we won't get a new connection, delete the socket
                    delete client;
                }
            } else {
                // The listener socket is not ready, test all other sockets (the clients)
                for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
                {
                    sf::TcpSocket& client = **it;
                    if (selector.isReady(client))
                    {
                        // The client has sent some data, we can receive it
                        sf::Packet packet;
                        sf::Uint32 api_command;
                        switch (client.receive(packet)) {
                            case sf::Socket::Done:
                                if (packet >> api_command && api_command == (sf::Uint32)APICommand::register_client) {
                                    RegisterClient(client);
                                }
                                break;
                            case sf::TcpSocket::Error:
                                std::cout << "TCP client encountered error. Removing client." << std::endl;
                                 selector.remove(client);
                                DeleteClient(&client, selector, clients);
                                break;
                            case sf::TcpSocket::Disconnected:
                                std::cout << "TCP client disconnected. Removing client. " << std::endl;
                                selector.remove(client);
                                DeleteClient(&client, selector, clients);
                                break;
                            default:
                                std::cout << "TCP client sent unkown signal." << std::endl;
                                break;
                        }
                    }
                }
            }
        }
    }
}

void NetworkingServer::DeleteClient(sf::TcpSocket* client, sf::SocketSelector selector, std::list<sf::TcpSocket*> clients) {
    clients.remove(client);
    mClientMoves.erase(mClientSocketsToIds.get(client));
    mClientSocketsToIds.erase(client);
}

void NetworkingServer::RegisterClient(sf::TcpSocket& client) {
    sf::Packet response_packet;
    sf::Uint32 api_command = (sf::Uint32)APICommand::register_client;
    if(response_packet << api_command << mClientIdCounter << (sf::Uint32) mCurrTick) {
        client.send(response_packet);
        std::cout << "Received client registration. Issued client ID " << mClientIdCounter << std::endl;
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

    client_inputs cis = {client_ids, client_direction_updates};
    return cis;
}

std::vector<int> NetworkingServer::getClientIds() {
    std::vector<int> client_ids;
    auto client_sockets_to_ids = mClientSocketsToIds.forceCopy();
    std::transform(
        client_sockets_to_ids.begin(), client_sockets_to_ids.end(), std::back_inserter(client_ids),
        [](std::pair<sf::TcpSocket*, sf::Int32> pair){return pair.second;}
    );
    return client_ids;
}

std::vector<client_direction_update> NetworkingServer::getClientDirectionUpdates() {
    std::vector<client_direction_update> client_direction_updates;
    for (const auto& client_move : mClientMoves.forceCopy()) {
        client_direction_update cd = {
            client_move.first, client_move.second.x_dir, client_move.second.y_dir};
        client_direction_updates.push_back(cd);
    }

    return client_direction_updates;
}


void NetworkingServer::setState(std::vector<std::shared_ptr<Group>> active_groups) {
    mClientGroupUpdates.clear();
    for(const auto active_group: active_groups) {
        sf::Uint32 client_id = active_group->getId();
        sf::Vector2f position = active_group->getCircle()->getPosition();
        float size = active_group->getCircle()->getRadius();
        client_group_update cgu = {client_id, position.x, position.y, size};
        mClientGroupUpdates.push_back(cgu);
    }
}

void NetworkingServer::incrementTick() {
    mCurrTick++;
}
