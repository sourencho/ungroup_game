#include "Player.hpp"
#include <iostream>
#include "../common/util.hpp"

Player::Player():mDirection(0.0, 0.0) {}

Player::~Player() {}

void Player::setDirection(sf::Vector2f direction) {
    mDirection = direction;
}

sf::Vector2f Player::getDirection() const {
    return mDirection;
}
