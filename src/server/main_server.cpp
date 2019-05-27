#include <SFML/Network.hpp>

#include <iostream>
#include <list>
#include <thread>
#include <future>
#include <chrono>
#include <unordered_map>

const unsigned int CMD_DRIFT_THRESHOLD = 5;
enum APICommand { register_client, group, ungroup };
enum RealtimeCommand { move, fetch_state };

std::unordered_map<sf::TcpSocket*, sf::Int32> client_sockets_to_ids;
std::unordered_map<sf::Uint32, float*> client_moves;
std::unordered_map<sf::Uint32, float*> client_positions;
sf::Uint32 client_id_counter = 0;

std::atomic<uint> curr_tick(0);
std::atomic<bool> accepting_move_commands(true);

void delete_client(sf::TcpSocket* client, sf::SocketSelector selector) {
  client_moves.erase(client_sockets_to_ids[client]);
  client_positions.erase(client_sockets_to_ids[client]);
  selector.remove(*client);
  delete client;
}

void api_server() {
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
                  if(response_packet << api_command << client_id_counter << (sf::Uint32)curr_tick) {
                    client.send(response_packet);
                    std::cout << "Received client registration. Issued client ID " << client_id_counter << std::endl;
                    client_positions[client_id_counter] = new float[2]{0.f, 0.f};
                    client_sockets_to_ids[&client] = client_id_counter;
                    client_id_counter++;
                  }
                }
                break;
              case sf::TcpSocket::Error:
                std::cout << "TCP client encountered error. Removing client." << std::endl;
                delete_client(&client, selector);
                break;
              case sf::TcpSocket::Disconnected:
                // clean up client_moves/client_positions hashes
                std::cout << "TCP client disconnected. Removing client. " << std::endl;
                delete_client(&client, selector);
                break;
            }
          }
        }
      }
    }
  }
}

void realtime_server() {
  sf::UdpSocket rt_server;
  rt_server.bind(4888);

  while (true) {
    sf::Uint32 ct = curr_tick;
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
            int drift = std::abs((int)((curr_tick - tick)));
            if (drift < CMD_DRIFT_THRESHOLD) {
              // your newest commands will overwrite old ones
              std::unordered_map<sf::Uint32, float*>::const_iterator iter = client_moves.find(client_id);
              if (accepting_move_commands) {
                // if entry exists, don't allocate new array.
                if(iter != client_moves.end()) {
                  iter->second[0] = x_dir;
                  iter->second[1] = y_dir;
                }
                // drop the move command if we're computing game state in the other thread. doesn't feel great.
                client_moves[client_id] = new float[2]{x_dir, y_dir};
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
          for (const auto iter : client_positions) {
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

// add instrumentation to make sure this is actually running near every 100 ms
void compute_game_state() {
  while (true) {
    accepting_move_commands = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    accepting_move_commands = false;
    // update game state, take moves array and simulate next tick given current state and inputs
    // ... simplest behavior would be to track client locations and apply vectors to them ...

    if (client_moves.size() == 0) {
      continue;
    }
    std::cout << "game state thread: dumping client moves for tick " << curr_tick << std::endl;
    for (const auto iter : client_moves) {
      sf::Uint32 client_id = iter.first;
      float* move_vector = iter.second;
      if (client_positions.find(client_id) != client_positions.end()) {
        client_positions[client_id][0] += move_vector[0]; // x component
        client_positions[client_id][1] += move_vector[1]; // y component
      }
      std::cout << iter.first << " " << iter.second[0] << " " << iter.second[1] << std::endl;
    }
    std::cout << "game state thread: dumping client positions for tick " << curr_tick << std::endl;
    for (const auto iter : client_positions) {
      std::cout << iter.first << " " << iter.second[0] << " " << iter.second[1] << std::endl;
    }
    // window is closed, time to reset. May want to use fix sized array as an optimization.
    // It's a bit mean to disguard old commands, maybe there should be windows for multiple ticks?
    client_moves.clear();
    curr_tick++;
  }
}

int main(int, char const**)
{
  std::cout << "Starting ungroup game server.\n";
  std::thread api_thread(api_server);
  std::thread realtime_thread(realtime_server);
  std::thread compute_game_state_thread(compute_game_state);
  api_thread.join();
  realtime_thread.join();
  compute_game_state_thread.join();

  return EXIT_SUCCESS;
}
