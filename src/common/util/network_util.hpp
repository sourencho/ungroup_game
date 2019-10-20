#ifndef network_util_hpp
#define network_util_hpp

#include "game_def.hpp"
#include <SFML/Network.hpp>

sf::Packet &operator<<(sf::Packet &packet, const UnreliableCommand &realtime_command);
sf::Packet &operator>>(sf::Packet &packet, UnreliableCommand &realtime_command);

sf::Packet &operator<<(sf::Packet &packet, const ReliableCommand &api_command);
sf::Packet &operator>>(sf::Packet &packet, ReliableCommand &api_command);

sf::Packet &operator<<(sf::Packet &packet, const sf::Vector2f &vector_2f);
sf::Packet &operator>>(sf::Packet &packet, sf::Vector2f &vector_2f);

sf::Socket::Status receiveWithTimeout(sf::TcpSocket &socket, sf::Packet &packet, sf::Time timeout);

#endif /* network_util_hpp */
