#include "network_util.hpp"

sf::Packet& operator <<(sf::Packet& packet, const RealtimeCommand& realtime_command) {
    return packet
        << realtime_command.client_id
        << realtime_command.command
        << realtime_command.tick;
}

sf::Packet& operator >>(sf::Packet& packet, RealtimeCommand& realtime_command) {
    return packet
        >> realtime_command.client_id
        >> realtime_command.command
        >> realtime_command.tick;
}


sf::Packet& operator <<(sf::Packet& packet, const ApiCommand& api_command) {
    return packet
        << api_command.client_id
        << api_command.command
        << api_command.tick;
}

sf::Packet& operator >>(sf::Packet& packet, ApiCommand& api_command) {
    return packet
        >> api_command.client_id
        >> api_command.command
        >> api_command.tick;
}

sf::Packet& operator <<(sf::Packet& packet, const sf::Vector2f& vector_2f) {
    return packet
        << vector_2f.x
        << vector_2f.y;
}

sf::Packet& operator >>(sf::Packet& packet, sf::Vector2f& vector_2f) {
    return packet
        >> vector_2f.x
        >> vector_2f.y;
}

sf::Packet& operator <<(sf::Packet& packet, const ClientGroupUpdate& client_group_update) {
    return packet
        << client_group_update.client_id
        << client_group_update.x_pos
        << client_group_update.y_pos
        << client_group_update.size;
}

sf::Packet& operator >>(sf::Packet& packet, ClientGroupUpdate& client_group_update) {
    return packet
        >> client_group_update.client_id
        >> client_group_update.x_pos
        >> client_group_update.y_pos
        >> client_group_update.size;
}
