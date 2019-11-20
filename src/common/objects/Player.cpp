#include "Player.hpp"

#include <iostream>

#include "../util/network_util.hpp"

Player::Player(uint32_t id) : GameObject(id), m_direction(0.0, 0.0), m_joinable(false) {
}

Player::~Player() {
}

/* Network Util */

PlayerUpdate Player::getUpdate() const {
    PlayerUpdate pu = {
        .player_id = static_cast<sf::Uint32>(getId()),
        .is_active = isActive(),
        .direction = getDirection(),
        .joinable = getJoinable(),
        .ungroup = getUngroup(),
    };
    return pu;
}

void Player::applyUpdate(PlayerUpdate pu) {
    setActive(pu.is_active);
    setDirection(pu.direction);
    setJoinable(pu.joinable);
    setUngroup(pu.ungroup);
}

sf::Packet& operator<<(sf::Packet& packet, const PlayerUpdate& player_update) {
    return packet << player_update.player_id << player_update.is_active << player_update.direction
                  << player_update.joinable << player_update.ungroup;
}

sf::Packet& operator>>(sf::Packet& packet, PlayerUpdate& player_update) {
    return packet >> player_update.player_id >> player_update.is_active >>
           player_update.direction >> player_update.joinable >> player_update.ungroup;
}