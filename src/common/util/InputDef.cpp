#include "InputDef.hpp"

sf::Packet& operator<<(sf::Packet& packet, const InputDef::UnreliableInput& cui) {
    return packet << cui.toggle_up << cui.toggle_down << cui.toggle_right << cui.toggle_left
                  << cui.toggle_stop;
}

sf::Packet& operator>>(sf::Packet& packet, InputDef::UnreliableInput& cui) {
    return packet >> cui.toggle_up >> cui.toggle_down >> cui.toggle_right >> cui.toggle_left >>
           cui.toggle_stop;
}

sf::Packet& operator<<(sf::Packet& packet, const InputDef::ReliableInput& cri) {
    return packet << cri.toggle_joinable << cri.toggle_ungroup;
}

sf::Packet& operator>>(sf::Packet& packet, InputDef::ReliableInput& cri) {
    return packet >> cri.toggle_joinable >> cri.toggle_ungroup;
}