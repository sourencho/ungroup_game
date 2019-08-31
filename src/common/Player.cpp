#include "Player.hpp"
#include <iostream>
#include "../common/util.hpp"

Player::Player(int id):
  GameObject(id), mDirection(0.0, 0.0), mGroupable(false) {}

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

PlayerUpdate Player::get_player_update() const {
    PlayerUpdate pu = {getId(), getDirection(), getGroupable()};
    return pu;
}

void Player::apply_player_update(const PlayerUpdate& pu) {
    setDirection(pu.direction);
    setGroupable(pu.groupable);
}
