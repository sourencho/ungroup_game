#include "InputDef.hpp"

sf::Packet& operator<<(sf::Packet& packet, const InputDef::UnreliableInput& ui) {
    return packet << ui.toggle_up << ui.toggle_down << ui.toggle_right << ui.toggle_left
                  << ui.toggle_stop;
}

sf::Packet& operator>>(sf::Packet& packet, InputDef::UnreliableInput& ui) {
    return packet >> ui.toggle_up >> ui.toggle_down >> ui.toggle_right >> ui.toggle_left >>
           ui.toggle_stop;
}

sf::Packet& operator<<(sf::Packet& packet, const InputDef::ReliableInput& ri) {
    return packet << ri.toggle_joinable << ri.toggle_ungroup << ri.toggle_intent;
}

sf::Packet& operator>>(sf::Packet& packet, InputDef::ReliableInput& ri) {
    return packet >> ri.toggle_joinable >> ri.toggle_ungroup >> ri.toggle_intent;
}