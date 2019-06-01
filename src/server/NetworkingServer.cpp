#include "NetworkingServer.hpp"

NetworkingServer::NetworkingServer() {
    mClientIdCounter = 0;
    mCurrTick = 0;
    mAcceptingMoveCommands = true;
    mAcceptingCircleReads = true;
    mAcceptingClientSocketToIdsReads = true;
}

NetworkingServer::~NetworkingServer() {
}

void NetworkingServer::Start() {
    std::cout << "Starting ungroup game server.\n";
    std::thread api_thread(&NetworkingServer::ApiServer, this);
    std::thread realtime_thread(&NetworkingServer::RealtimeServer, this);
    std::thread compute_game_state_thread(&NetworkingServer::ComputeGameState, this);
    api_thread.detach();
    realtime_thread.detach();
    compute_game_state_thread.detach();
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
                    break;
                case (sf::Uint32)RealtimeCommand::fetch_state:
                    //std::cout << "Sending game state to client: " << sender << " " << port << std::endl;
                    // sample current state every 100 ms, this simply packages and returns it
                    game_state_packet << ct; // should have error handling for <<
                    if (mAcceptingCircleReads) {
                        for(const auto circle: mCircles) {
                            game_state_packet << circle.id << circle.x_pos << circle.y_pos << circle.size;
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
                                if (packet >> api_command && (api_command == (sf::Uint32)APICommand::register_client)) {
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
                                break;
                        }
                    }
                }
            }
        }
    }
}

void NetworkingServer::ComputeGameState() {
    while (true) {
        mAcceptingMoveCommands = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mAcceptingMoveCommands = false;

        // window is closed, time to reset. May want to use fix sized array as an optimization.
        // It's a bit mean to disguard old commands, maybe there should be windows for multiple ticks?
        mCurrTick++;
        // This sleep is needed to give the GameController time to fetch the client directions in getClientDirections
        // TODO: Not sure what this sleep value should be...
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void NetworkingServer::DeleteClient(sf::TcpSocket* client, sf::SocketSelector selector) {
    mClientMoves.erase(mClientSocketsToIds[client]);
    selector.remove(*client);
    delete client;
}

std::vector<client_direction> NetworkingServer::getClientDirections() {
    std::vector<client_direction> client_directions;
    if (mAcceptingMoveCommands == false) {
        for(const auto& client_move : mClientMoves) {
            client_direction cd = {client_move.first, client_move.second[0], client_move.second[1]};
            client_directions.push_back(cd);
        }
    }
    return client_directions;
}

void NetworkingServer::setState(std::vector<Group*> active_groups) {
    mAcceptingCircleReads = false;
    mCircles.clear();
    for(const auto active_group: active_groups) {
        sf::Uint32 client_id = active_group->getId();
        sf::Vector2f position = active_group->getPosition();
        float size = active_group->getSize();
        circle c = {client_id, position.x, position.y, size};
        mCircles.push_back(c);
    }
    mAcceptingCircleReads = true;
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
