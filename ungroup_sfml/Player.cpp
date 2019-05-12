#include "Player.hpp"
#include "util.hpp"
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
        mDirection = sf::Vector2f(0.f,0.f);
        if(sf::Keyboard::isKeyPressed(mUp)) { mDirection += sf::Vector2f(0.f,-1.f); }
        if(sf::Keyboard::isKeyPressed(mDown)) { mDirection += sf::Vector2f(0.f,1.f); }
        if(sf::Keyboard::isKeyPressed(mLeft)) { mDirection += sf::Vector2f(-1.f,0.f); }
        if(sf::Keyboard::isKeyPressed(mRigth)) { mDirection += sf::Vector2f(1.f,0.f); }
        mDirection = normalize(mDirection);
    }
}

