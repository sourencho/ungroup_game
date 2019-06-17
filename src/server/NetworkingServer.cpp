#include "NetworkingServer.hpp"

NetworkingServer::NetworkingServer():mCurrTick(0) {
    mClientInputsWriteLock.unlock();
    mClientGroupUpdatesWriteLock.unlock();

    std::cout << "Starting ungroup game server.\n";
    std::thread api_thread(&NetworkingServer::ApiServer, this);
    std::thread realtime_thread(&NetworkingServer::RealtimeServer, this);
    api_thread.detach();
    realtime_thread.detach();
}

NetworkingServer::~NetworkingServer() {}

void NetworkingServer::collectClientInputs() {
    mClientInputsWriteLock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mClientInputsWriteLock.lock();
}

void NetworkingServer::Move(sf::Packet command_packet, sf::Uint32 client_id, sf::Uint32 tick) {
    mClientInputsWriteLock.lock();

    float x_dir;
    float y_dir;
    if (command_packet >> x_dir >> y_dir) {
        int drift = std::abs((int)((mCurrTick - tick)));
        if (drift < CMD_DRIFT_THRESHOLD) {
            // your newest commands will overwrite old ones
            std::unordered_map<sf::Uint32, float*>::const_iterator iter = mClientMoves.find(client_id);
            // if entry exists, don't allocate new array.
            if(iter != mClientMoves.end()) {
                iter->second[0] = x_dir;
                iter->second[1] = y_dir;
            }
            // drop the move command if we're computing game state in the other thread. doesn't feel great.
            mClientMoves[client_id] = new float[2]{x_dir, y_dir};
        } else {
            std::cout << "Receive move command with tick drifted past drift threshold. Drift value is: "
                << drift << std::endl;
        }
    }

    mClientInputsWriteLock.unlock();
}

void NetworkingServer::RegisterClient(sf::TcpSocket& client) {
    mClientInputsWriteLock.lock();

    sf::Packet response_packet;
    sf::Uint32 api_command = (sf::Uint32)APICommand::register_client;
    if(response_packet << api_command << mClientIdCounter << (sf::Uint32) mCurrTick) {
        client.send(response_packet);
        std::cout << "Received client registration. Issued client ID " << mClientIdCounter << std::endl;

        mClientSocketsToIds[&client] = mClientIdCounter;

        mClientIdCounter++;
    }

    mClientInputsWriteLock.unlock();
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
                    mClientGroupUpdatesWriteLock.lock();
                    for(const auto group_circle_update: mClientGroupUpdates) {
                        game_state_packet << group_circle_update.client_id
                            << group_circle_update.x_pos
                            << group_circle_update.y_pos
                            << group_circle_update.size;
                    }
                    mClientGroupUpdatesWriteLock.unlock();
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
                                 selector.remove(client);
                                DeleteClient(&client, selector);
                                break;
                            case sf::TcpSocket::Disconnected:
                                // clean up mClientMoves/mClientPositions hashes
                                std::cout << "TCP client disconnected. Removing client. " << std::endl;
                                selector.remove(client);
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
    mClientInputsWriteLock.lock();

    delete client;
    mClientMoves.erase(mClientSocketsToIds[client]);
    mClientSocketsToIds.erase(client);

    mClientInputsWriteLock.unlock();
}



void NetworkingServer::setState(std::vector<std::shared_ptr<Group>> active_groups) {
    mClientGroupUpdatesWriteLock.lock();

    mClientGroupUpdates.clear();
    for(const auto active_group: active_groups) {
        sf::Uint32 client_id = active_group->getId();
        sf::Vector2f position = active_group->getCircle()->getPosition();
        float size = active_group->getCircle()->getRadius();
        group_circle_update gcu = {client_id, position.x, position.y, size};
        mClientGroupUpdates.push_back(gcu);
    }

    mClientGroupUpdatesWriteLock.unlock();
}

// GAME_CONTROLLER THREAD FUNCTIONS

client_inputs NetworkingServer::getClientInputs() {
    std::vector<int> client_ids = getClientIds();
    std::vector<client_direction_update> client_direction_updates = getClientDirectionUpdates();

    client_inputs cis = {client_ids, client_direction_updates};
    return cis;
}

std::vector<int> NetworkingServer::getClientIds() {
    // Read mClientSocketsToIds
    std::vector<int> client_ids;
    std::transform(
        mClientSocketsToIds.begin(), mClientSocketsToIds.end(), std::back_inserter(client_ids),
        [](std::pair<sf::TcpSocket*, sf::Int32> pair){return pair.second;}
    );
    return client_ids;
}

std::vector<client_direction_update> NetworkingServer::getClientDirectionUpdates() {
    // Read mClientMoves
    std::vector<client_direction_update> client_direction_updates;
    for(const auto& client_move : mClientMoves) {
        client_direction_update cd = {client_move.first, client_move.second[0], client_move.second[1]};
        client_direction_updates.push_back(cd);
    }

    return client_direction_updates;
}

void NetworkingServer::incrementTick() {
    mCurrTick++;
}
