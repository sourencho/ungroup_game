#include "Player.hpp"

#include <iostream>

#include "../common/util.hpp"
#include "../common/network_util.hpp"

sf::Packet& operator <<(sf::Packet& packet, const PlayerUpdate& player_update) {
    return packet
        << player_update.player_id
        << player_update.direction
        << player_update.groupable;
}

sf::Packet& operator >>(sf::Packet& packet, PlayerUpdate& player_update) {
    return packet
        >> player_update.player_id
        >> player_update.direction
        >> player_update.groupable;
}

Player::Player(int id):
  GameObject(id), mDirection(1.0, 1.0), mGroupable(false) {}

Player::~Player() {}

void Player::setDirection(sf::Vector2f direction) {
    mDirection = direction;
}

void Player::setGroupable(bool groupable) {
    mGroupable = groupable;
}

sf::Vector2f Player::getDirection() const {
    return mDirection;
}

bool Player::getGroupable() const {
    return mGroupable;
}

void Player::toggleGroupable() {
    mGroupable ^= true;
}

PlayerUpdate Player::getUpdate() const {
    PlayerUpdate pu = {(sf::Uint32) getId(), getDirection(), getGroupable()};
    return pu;
}

void Player::applyUpdate(const PlayerUpdate& pu) {
    setDirection(pu.direction);
    setGroupable(pu.groupable);
}
