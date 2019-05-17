#include <SFML/Network.hpp>

#include <iostream>
#include <list>

int main(int, char const**)
{
  // Create a socket to listen to new connections
  sf::TcpListener listener;
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
            if (client.receive(packet) == sf::Socket::Done)
            {
              // Extract the variables contained in the packet
              sf::Uint32 tick;
              sf::Uint32 x_dir;
              sf::Uint32 y_dir;
              if (packet >> tick >> x_dir >> y_dir)
              {
                  // Data extracted successfully...
                std::cout << packet;
              }
            }
          }
        }
      }
    }
  }
  return EXIT_SUCCESS;
}
