#include "Player.hpp"
#include "../common/util.hpp"
#include <iostream>

Player::Player():mDirection(0.0, 0.0) {
    //ctor
}

Player::~Player() {
    //dtor
}

void Player::setDirection(sf::Vector2f direction) {
    mDirection = direction;
}

sf::Vector2f Player::getDirection() const {
    return mDirection;
}
