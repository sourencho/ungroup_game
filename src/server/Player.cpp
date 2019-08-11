#include "Player.hpp"
#include <iostream>
#include "../common/util.hpp"

Player::Player():GameObject(0), mDirection(0.0, 0.0), mGroupable(false) {}

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
