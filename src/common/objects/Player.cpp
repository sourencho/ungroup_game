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

Player::Player(uint32_t id) : GameObject(id), mDirection(1.0, 1.0), mJoinable(false) {}

Player::~Player() {}

void Player::setDirection(sf::Vector2f direction) { mDirection = direction; }

void Player::setJoinable(bool joinable) { mJoinable = joinable; }

sf::Vector2f Player::getDirection() const { return mDirection; }

bool Player::getJoinable() const { return mJoinable; }

PlayerUpdate Player::getUpdate() const {
    PlayerUpdate pu = {static_cast<sf::Uint32>(getId()), isActive(), getDirection(), getJoinable()};
    return pu;
}

void Player::applyUpdate(PlayerUpdate pu) {
    setActive(pu.is_active);
    setDirection(pu.direction);
    setJoinable(pu.joinable);
}
