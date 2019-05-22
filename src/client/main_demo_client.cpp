#include <SFML/Network.hpp>

#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>

int my_group_id;
sf::Uint32 curr_tick;
sf::Uint32 serv_x_pos;
sf::Uint32 serv_y_pos;

void recv(sf::TcpSocket& s, std::mutex& mutex)
{
  std::cout << "Entered receive thread.\n";
  while (true) {
    sf::Packet packet;
    sf::Uint32 tick;
    sf::Uint32 group_id;
    sf::Uint32 x_pos;
    sf::Uint32 y_pos;
    sf::Uint32 x_dir;
    sf::Uint32 y_dir;

    std::cout << "Receive entered\n";
    if (s.receive(packet) == sf::Socket::Done) {
      std::cout << "Receive successful!\n";

      if (packet >> group_id >> tick >> x_dir >> y_dir >> x_pos >> y_pos) {
        std::cout << "Received group_id:" << group_id << " tick:" << tick << " x:" <<  x_pos << " y:"<< y_pos << "\n";
        // update own position to server view
        if (my_group_id == group_id) {
          curr_tick = tick;
          serv_x_pos = x_pos;
          serv_y_pos = y_pos;
          std::cout << "Adjusting to server values for position\n";
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void send(sf::TcpSocket& s, std::mutex& mutex)
{
  std::cout << "Entered sending thread.\n";
  while (true) {
    sf::Packet packet;
    curr_tick++;
    std::cout << "LOCKED IN SEND.\n";
    sf::Uint32 tick = curr_tick;
    sf::Uint32 group_id = (sf::Uint32) my_group_id;
    sf::Uint32 x_pos = serv_x_pos + 1;
    sf::Uint32 y_pos = serv_y_pos + 1;
    std::cout << "UNLOCKED IN SEND.\n";
    sf::Uint32 x_dir = 0;
    sf::Uint32 y_dir = 0;
    //std::cout << "Crafting packet to send with " << group_id << tick << x_dir << y_dir << x_pos << y_pos << "\n";
    if (packet << group_id << tick << x_dir << y_dir << x_pos << y_pos)
    {
      std::cout << "Sending group_id:" << group_id << " tick:" << tick << " x_dir:" << x_dir << " y_dir:" << y_dir << " x_pos:" << x_pos << " y_pos:" << y_pos  << "\n";
      s.send(packet);
    }
    std::cout << "SLEEP IN SEND.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

int main(int, char const**)
{
  // not a great way to do this, server should dispense these or use a UUID lib
  std::srand(std::time(nullptr));
  my_group_id = std::rand();

  sf::TcpSocket socket;
  socket.connect("127.0.0.1", 4844);

  std::cout << "Connected. Starting threads.\n";

  std::mutex mutex;
  std::thread t1(recv, std::ref(socket), std::ref(mutex));
  std::thread t2(send, std::ref(socket), std::ref(mutex));
  t1.join();
  t2.join();

  return EXIT_SUCCESS;
}
