#include "NetworkingServer.hpp"

NetworkingServer::NetworkingServer() {
    mClientIdCounter = 0;
    mCurrTick = 0;
    mAcceptingMoveCommands = true;
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
                        std::cout << "Client ID, tick, command, x, y: " << client_id << " " << tick << " " << realtime_command << " " << x_dir << " " << y_dir << std::endl;
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
                                std::cout << "Dropping move command because game state being computed" << std::endl;
                            }
                        } else {
                            std::cout << "Receive move command with tick drifted past drift threshold. Drift value is: " << drift << std::endl;
                        }
                    }
                    break;
                case (sf::Uint32)RealtimeCommand::fetch_state:
                    std::cout << "Sending game state to client: " << sender << " " << port << std::endl;
                    // sample current state every 100 ms, this simply packages and returns it
                    game_state_packet << ct; // should have error handling for <<
                    for (const auto iter : mClientPositions) {
                        sf::Uint32 client_id = iter.first;
                        game_state_packet << client_id << iter.second[0] << iter.second[1];
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

    // Create a list to store the future clients
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
                                        mClientPositions[mClientIdCounter] = new float[2]{0.f, 0.f};
                                        mClientSocketsToIds[&client] = mClientIdCounter;
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
        // update game state, take moves array and simulate next tick given current state and inputs
        // ... simplest behavior would be to track client locations and apply vectors to them ...

        if (mClientMoves.size() == 0) {
            continue;
        }
        std::cout << "game state thread: dumping client moves for tick " << mCurrTick << std::endl;
        for (const auto iter : mClientMoves) {
            sf::Uint32 client_id = iter.first;
            float* move_vector = iter.second;
            if (mClientPositions.find(client_id) != mClientPositions.end()) {
                mClientPositions[client_id][0] += move_vector[0]*5.f; // x component
                mClientPositions[client_id][1] += move_vector[1]*5.f; // y component
            }
            std::cout << iter.first << " " << iter.second[0] << " " << iter.second[1] << std::endl;
        }
        std::cout << "game state thread: dumping client positions for tick " << mCurrTick << std::endl;
        for (const auto iter : mClientPositions) {
            std::cout << iter.first << " " << iter.second[0] << " " << iter.second[1] << std::endl;
        }
        // window is closed, time to reset. May want to use fix sized array as an optimization.
        // It's a bit mean to disguard old commands, maybe there should be windows for multiple ticks?
        mClientMoves.clear();
        mCurrTick++;
    }
}

void NetworkingServer::DeleteClient(sf::TcpSocket* client, sf::SocketSelector selector) {
    mClientMoves.erase(mClientSocketsToIds[client]);
    mClientPositions.erase(mClientSocketsToIds[client]);
    selector.remove(*client);
    delete client;
}
