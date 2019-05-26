#include <SFML/Network.hpp>

#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>

// dedup these into shared header file
enum class APICommand { register_client, group, ungroup };
enum class RealtimeCommand { move, fetch_state };

sf::Uint32 my_client_id;
sf::Uint32 curr_tick;
// these values should be user input instead of hard-coded
sf::Uint32 x_dir = 1;
sf::Uint32 y_dir = 1;

bool is_registered = false;

void read_registration_response(sf::TcpSocket* api_client) {
  // read registration data
  sf::Packet registration_response;
  sf::Uint32 api_command;
  sf::Uint32 client_id;
  sf::Uint32 tick;

  if (api_client->receive(registration_response) == sf::Socket::Done) {
    if (registration_response >> api_command >> client_id >> tick && (api_command == (sf::Uint32)APICommand::register_client)) {
      std::cout << "Registered with ID and current tick: " << client_id << " " << tick << std::endl;
      my_client_id = client_id;
      curr_tick = tick;
      is_registered = true;
    }
  }
}

// these will be strictly for ungroup/group/register/deregister and other Big abstraction changes
// not sure it's really necessary, but implementing retry logic for something like ungrouping specifically
// would be annoying. We're a lot more OK with duplication/out of order sends/dropped datagrams for stuff like moving.
void api_client_recv(sf::TcpSocket* api_client) {}
void api_client_send(sf::TcpSocket* api_client) {}

void realtime_client_recv(sf::UdpSocket* realtime_client) {
  while (true) {
    sf::Packet packet;
    sf::IpAddress sender;
    unsigned short port;
    if(!realtime_client->receive(packet, sender, port)) { continue; };
    // fetch state updates for now
    sf::Uint32 server_tick;
    sf::Uint32 client_id;
    sf::Uint32 x_pos;
    sf::Uint32 y_pos;
    if (packet >> server_tick) {
      while (packet >> client_id >> x_pos >> y_pos) {
        std::cout << "Client: " << client_id << " has position x,y: " << x_pos << " " << y_pos << std::endl;
      }
      // Im not sure what kind of synchronization needs to happen here.
      // If this tick is the most up-to-date we've ever seen, maybe we set the game to it?
      std::cout << "Client positions from server for tick: " << server_tick << std::endl;
      curr_tick = server_tick;
    } else {
      std::cout << "Failed to read server tick from new packet" << std::endl;
    }
  }
}

void realtime_client_send(sf::UdpSocket* realtime_client) {
  while (true) {
    sf::Packet packet;
    sf::Uint32 move_cmd = (sf::Uint32)RealtimeCommand::move;
    if (packet << my_client_id << move_cmd << curr_tick << x_dir << y_dir) {
      realtime_client->send(packet,"127.0.0.1", 4845);
    } else {
      std::cout << "Failed to form packet" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
  }
}

// this probably should just be in realtime_client_send and realtime_client_send
// should poll client state to see if the current tick has a move AND if the move has
// been sent to the server yet. If both are true, then it should try to communicate the move.
// This would decouple local move setting commands from the network communication. They'd be happening
// in different threads, which is a Good Thing(tm) to avoid blocking in the client on network IO.
void sync_server_state(sf::UdpSocket* realtime_client) {
  while (true) {
    sf::Packet packet;
    sf::Uint32 fetch_state_cmd = (sf::Uint32)RealtimeCommand::fetch_state;
    if (packet << my_client_id << fetch_state_cmd << curr_tick) {
      realtime_client->send(packet, "127.0.0.1", 4888);
    } else {
      std::cout << "Failed to form packet" << std::endl;
    }
    // fetch state constantly
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

int main(int, char const**)
{
  std::cout << "Starting ungroup demo client." << std::endl;
  sf::TcpSocket api_client;
  api_client.connect("127.0.0.1", 4844);
  sf::Packet registration_request;
  if(registration_request << (sf::Uint32)APICommand::register_client) {
    api_client.send(registration_request);
    read_registration_response(&api_client);
  }

  if (!is_registered) {
    std::cout << "Failed to register. Exiting." << std::endl;
    return EXIT_FAILURE;
  }

  sf::UdpSocket realtime_client;
  realtime_client.bind(sf::Socket::AnyPort);
/*
  // api
  std::thread api_client_recv_thread(api_client_recv, &api_client);
  std::thread api_client_send_thread(api_client_send, &api_client);
*/
  // realtime
  std::thread realtime_client_recv_thread(realtime_client_recv, &realtime_client);
  std::thread realtime_client_send_thread(realtime_client_send, &realtime_client);

  // syncs authoritative sever state to client at a regular interval
  std::thread sync_server_state_thread(sync_server_state, &realtime_client);

  // Only really need one of these joins since they're all in infinite loops
  // api_client_recv_thread.join();
  // api_client_send_thread.join();
  realtime_client_recv_thread.join();
  realtime_client_send_thread.join();
  sync_server_state_thread.join();

  return EXIT_SUCCESS;
}
