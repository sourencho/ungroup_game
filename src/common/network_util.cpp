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

sf::Packet& operator <<(sf::Packet& packet, const GroupUpdate& group_update) {
    return packet
        << group_update.group_id
        << group_update.x_pos
        << group_update.y_pos
        << group_update.radius;
}

sf::Packet& operator >>(sf::Packet& packet, GroupUpdate& group_update) {
    return packet
        >> group_update.group_id
        >> group_update.x_pos
        >> group_update.y_pos
        >> group_update.radius;
}

sf::Packet& operator <<(sf::Packet& packet, const MineUpdate& mine_update) {
    return packet
        << mine_update.mine_id
        << mine_update.x_pos
        << mine_update.y_pos
        << mine_update.radius;
}

sf::Packet& operator >>(sf::Packet& packet, MineUpdate& mine_update) {
    return packet
        >> mine_update.mine_id
        >> mine_update.x_pos
        >> mine_update.y_pos
        >> mine_update.radius;
}
