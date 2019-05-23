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
    mKeys.up = keys[0];
    mKeys.down = keys[1];
    mKeys.right = keys[2];
    mKeys.left = keys[3];
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

void Client::update(std::vector<Group*> active_groups) {
    for(auto active_group: active_groups) {
        int active_group_id = active_group->getId();

        if (active_group_id >= mGroupShapes.size()) {
            throw std::runtime_error("Update group with no corresponding GroupShape");
        }

        GroupShape* group_shape = mGroupShapes[active_group_id];

        group_shape->setPosition(active_group->getPosition());
        group_shape->setRadius(active_group->getSize());
        group_shape->setActive(true);
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
        if(sf::Keyboard::isKeyPressed(mKeys.up)) {
            mDirection += sf::Vector2f(0.f,-1.f);
        }
        if(sf::Keyboard::isKeyPressed(mKeys.down)) {
            mDirection += sf::Vector2f(0.f,1.f);
        }
        if(sf::Keyboard::isKeyPressed(mKeys.left)) {
            mDirection += sf::Vector2f(-1.f,0.f);
        }
        if(sf::Keyboard::isKeyPressed(mKeys.right)) {
            mDirection += sf::Vector2f(1.f,0.f);
        }
        mDirection = normalize(mDirection);
    }
}
