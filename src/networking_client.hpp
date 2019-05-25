#ifndef networking_client_hpp
#define networking_client_hpp

#include <stdio.h>
#include <SFML/Network.hpp>

sf::TcpSocket* create_api_client();
sf::UdpSocket* create_realtime_client();
sf::Uint32 register_networking_client(sf::TcpSocket* api_client);
int start_networking_client(sf::TcpSocket* api_client, sf::UdpSocket* realtime_client);

#endif /* networking_client_hpp */
