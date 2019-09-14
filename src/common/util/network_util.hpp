#ifndef network_util_hpp
#define network_util_hpp

#include <SFML/Network.hpp>
#include "game_def.hpp"

sf::Packet& operator <<(sf::Packet& packet, const UnreliableCommand& realtime_command);
sf::Packet& operator >>(sf::Packet& packet, UnreliableCommand& realtime_command);

sf::Packet& operator <<(sf::Packet& packet, const ReliableCommand& api_command);
sf::Packet& operator >>(sf::Packet& packet, ReliableCommand& api_command);

sf::Packet& operator <<(sf::Packet& packet, const sf::Vector2f& vector_2f);
sf::Packet& operator >>(sf::Packet& packet, sf::Vector2f& vector_2f);

sf::Packet& operator <<(sf::Packet& packet, const PlayerId& player_id);
sf::Packet& operator >>(sf::Packet& packet, PlayerId& player_id);

#endif /* network_util_hpp */
