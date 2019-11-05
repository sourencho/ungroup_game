#include "Player.hpp"

#include <iostream>

#include "../util/network_util.hpp"

sf::Packet& operator<<(sf::Packet& packet, const PlayerUpdate& player_update) {
    return packet << player_update.player_id << player_update.is_active << player_update.direction
                  << player_update.joinable;
}

sf::Packet& operator>>(sf::Packet& packet, PlayerUpdate& player_update) {
    return packet >> player_update.player_id >> player_update.is_active >>
           player_update.direction >> player_update.joinable;
}

Player::Player(uint32_t id) : GameObject(id), m_direction(1.0, 1.0), m_joinable(false) {}

Player::~Player() {}

void Player::setDirection(sf::Vector2f direction) { m_direction = direction; }

void Player::setJoinable(bool joinable) { m_joinable = joinable; }

sf::Vector2f Player::getDirection() const { return m_direction; }

bool Player::getJoinable() const { return m_joinable; }

PlayerUpdate Player::getUpdate() const {
    PlayerUpdate pu = {static_cast<sf::Uint32>(getId()), isActive(), getDirection(), getJoinable()};
    return pu;
}

void Player::applyUpdate(PlayerUpdate pu) {
    setActive(pu.is_active);
    setDirection(pu.direction);
    setJoinable(pu.joinable);
}
