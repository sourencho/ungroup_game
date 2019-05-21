#include <stdio.h>
#include <iostream>
#include "Client.hpp"
#include "util.hpp"

Client::Client(int max_player_count, sf::Keyboard::Key keys[4]):mDirection(1.0, 1.0) {
    for (int i=0; i < max_player_count; i++) {
        mGroupShapes.push_back(
            new GroupShape(
                1.f,
                sf::Vector2f(10.f, 10.f),
                sf::Color(rand() % 255, rand() % 255, rand() % 255)
            )
        );
    }

    // Set up input
    mUp = keys[0];
    mDown = keys[1];
    mRigth = keys[2];
    mLeft = keys[3];
}

Client::~Client() {
    //dtor
}

void Client::draw(sf::RenderTarget& target) {
    // Draw all groupCircles
    for (auto group_shape: mGroupShapes) {
        if (group_shape->isActive()) {
            group_shape->draw(target);
        }
    }
}

void Client::update(std::vector<Group*> groups) {
    for(auto group: groups) {
        int group_id = group->getId();
        if (group->isActive()) {
            GroupShape* group_circle = mGroupShapes[group->getId()];

            group_circle->setPosition(group->getPosition());
            group_circle->setRadius(group->getSize());
            group_circle->setActive(true);
        }
    }
}

sf::Vector2f Client::getDirection() const {
    return mDirection;
}

void Client::setId(int id) {
    mId = id;
}


int Client::getId() const {
    return mId;
}

void Client::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        mDirection = sf::Vector2f(0.f,0.f);
        if(sf::Keyboard::isKeyPressed(mUp)) {
            mDirection += sf::Vector2f(0.f,-1.f);
        }
        if(sf::Keyboard::isKeyPressed(mDown)) {
            mDirection += sf::Vector2f(0.f,1.f);
        }
        if(sf::Keyboard::isKeyPressed(mLeft)) {
            mDirection += sf::Vector2f(-1.f,0.f);
        }
        if(sf::Keyboard::isKeyPressed(mRigth)) {
            mDirection += sf::Vector2f(1.f,0.f);
        }
        mDirection = normalize(mDirection);
    }
}
