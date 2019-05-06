//
//  Player.cpp
//  ungroup_sfml
//
//  Created by Souren Papazian on 5/5/19.
//  Copyright © 2019 Souren Papazian. All rights reserved.
//

#include "Player.hpp"
#include <iostream>

Player::Player(sf::Keyboard::Key keys[4]):mDirection(1.0, 1.0) {
    mUp = keys[0];
    mDown = keys[1];
    mRigth = keys[2];
    mLeft = keys[3];
}

Player::~Player() {
    //dtor
}

void Player::update() {
    //
}

sf::Vector2f Player::getDirection() const {
    return mDirection;
}

void Player::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == mUp) {
            mDirection.x = 0.f;
            mDirection.y = -1.f;
        } else if (event.key.code == mDown) {
            mDirection.x = 0.f;
            mDirection.y = 1.f;
        }
        if (event.key.code == mLeft) {
            mDirection.x = -1.f;
            mDirection.y = 0.f;
        } else if (event.key.code == mRigth) {
            mDirection.x = 1.f;
            mDirection.y = 0.f;
        }
    }
}

