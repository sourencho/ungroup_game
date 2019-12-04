#include "network_util.hpp"

sf::Packet& operator<<(sf::Packet& packet, const UnreliableCommand& realtime_command) {
    return packet << realtime_command.client_id << realtime_command.command
                  << realtime_command.tick;
}

sf::Packet& operator>>(sf::Packet& packet, UnreliableCommand& realtime_command) {
    return packet >> realtime_command.client_id >> realtime_command.command >>
           realtime_command.tick;
}

sf::Packet& operator<<(sf::Packet& packet, const ReliableCommand& api_command) {
    return packet << api_command.client_id << api_command.command << api_command.tick;
}

sf::Packet& operator>>(sf::Packet& packet, ReliableCommand& api_command) {
    return packet >> api_command.client_id >> api_command.command >> api_command.tick;
}

sf::Packet& operator<<(sf::Packet& packet, const sf::Vector2f& vector_2f) {
    return packet << vector_2f.x << vector_2f.y;
}

sf::Packet& operator>>(sf::Packet& packet, sf::Vector2f& vector_2f) {
    return packet >> vector_2f.x >> vector_2f.y;
}

sf::Packet& operator<<(sf::Packet& packet, const State& state) {
    return packet << static_cast<sf::Uint32>(state);
}

sf::Packet& operator>>(sf::Packet& packet, State& state) {
    sf::Uint32 state_index;
    packet >> state_index;
    state = State(state_index);
    return packet;
}

sf::Socket::Status receiveWithTimeout(sf::TcpSocket& socket, sf::Packet& packet, sf::Time timeout) {
    sf::SocketSelector selector;
    selector.add(socket);
    if (selector.wait(timeout))
        return socket.receive(packet);
    else
        return sf::Socket::NotReady;
}
