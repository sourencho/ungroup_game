#include "NetworkingServer.hpp"

NetworkingServer::NetworkingServer() {
    mClientIdCounter = 0;
    mCurrTick = 0;
    mAcceptingMoveCommands = true;
    mAcceptingNetworkGameObjectsReads = true;
    mAcceptingClientSocketToIdsReads = true;
}

NetworkingServer::~NetworkingServer() {}

void NetworkingServer::Start() {
    std::cout << "Starting ungroup game server.\n";
    std::thread api_thread(&NetworkingServer::ApiServer, this);
    std::thread realtime_thread(&NetworkingServer::RealtimeServer, this);
    api_thread.detach();
    realtime_thread.detach();
}

void NetworkingServer::Move(sf::Packet command_packet, sf::Uint32 client_id, sf::Uint32 tick) {
    float x_dir;
    float y_dir;
    if (command_packet >> x_dir >> y_dir) {
        //std::cout << "Client ID, tick, command, x, y: " << client_id << " " << tick << " " << realtime_command << " " << x_dir << " " << y_dir << std::endl;
        int drift = std::abs((int)((mCurrTick - tick)));
        if (drift < CMD_DRIFT_THRESHOLD) {
            // your newest commands will overwrite old ones
            std::unordered_map<sf::Uint32, float*>::const_iterator iter = mClientMoves.find(client_id);
            if (mAcceptingMoveCommands) {
                // if entry exists, don't allocate new array.
                if(iter != mClientMoves.end()) {
                    iter->second[0] = x_dir;
                    iter->second[1] = y_dir;
                }
                // drop the move command if we're computing game state in the other thread. doesn't feel great.
                mClientMoves[client_id] = new float[2]{x_dir, y_dir};
            } else {
                //std::cout << "Dropping move command because game state being computed" << std::endl;
            }
        } else {
            std::cout << "Receive move command with tick drifted past drift threshold. Drift value is: " << drift << std::endl;
        }
    }
}

void NetworkingServer::RegisterClient(sf::TcpSocket& client) {
    sf::Packet response_packet;
    sf::Uint32 api_command = (sf::Uint32)APICommand::register_client;
    if(response_packet << api_command << mClientIdCounter << (sf::Uint32) mCurrTick) {
        client.send(response_packet);
        std::cout << "Received client registration. Issued client ID " << mClientIdCounter << std::endl;
        mAcceptingClientSocketToIdsReads = false;
        mClientSocketsToIds[&client] = mClientIdCounter;
        mAcceptingClientSocketToIdsReads = true;
        mClientIdCounter++;
    }
}

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
                    if (mAcceptingNetworkGameObjectsReads) {
                        for(const auto network_game_object: mNetworkGameObjects) {
                            game_state_packet << network_game_object.id
                                << network_game_object.x_pos
                                << network_game_object.y_pos
                                << network_game_object.size;
                        }
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
                                DeleteClient(&client, selector);
                                break;
                            case sf::TcpSocket::Disconnected:
                                // clean up mClientMoves/mClientPositions hashes
                                std::cout << "TCP client disconnected. Removing client. " << std::endl;
                                DeleteClient(&client, selector);
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

void NetworkingServer::DeleteClient(sf::TcpSocket* client, sf::SocketSelector selector) {
    mClientMoves.erase(mClientSocketsToIds[client]);
    selector.remove(*client);
    mClientSocketsToIds.erase(client);
    delete client;
}

std::vector<network_player> NetworkingServer::getNetworkPlayers() {
    std::vector<network_player> network_players;
    if (mAcceptingMoveCommands == false) {
        for(const auto& client_move : mClientMoves) {
            network_player np = {client_move.first, client_move.second[0], client_move.second[1]};
            network_players.push_back(np);
        }
    }
    return network_players;
}

void NetworkingServer::setState(std::vector<Group*> active_groups) {
    mAcceptingNetworkGameObjectsReads = false;
    mNetworkGameObjects.clear();
    for(const auto active_group: active_groups) {
        sf::Uint32 client_id = active_group->getId();
        sf::Vector2f position = active_group->getPosition();
        float size = active_group->getSize();
        network_game_object ngo = {client_id, position.x, position.y, size};
        mNetworkGameObjects.push_back(ngo);
    }
    mAcceptingNetworkGameObjectsReads = true;
}

std::vector<int> NetworkingServer::getClientIds() {
    std::vector<int> client_ids;
    if (mAcceptingClientSocketToIdsReads) {
        for(const auto& x : mClientSocketsToIds) {
            client_ids.push_back(x.second);
        }
    }
    return client_ids;
}

void NetworkingServer::setAcceptingMoveCommands(bool accepting_move_commands) {
    mAcceptingMoveCommands = accepting_move_commands;
}

void NetworkingServer::incrementTick() {
    mCurrTick++;
}
