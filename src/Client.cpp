#include <stdio.h>
#include <iostream>
#include "Client.hpp"

Client::Client(int player_count) {
    for (int i=0; i < player_count; i++) {
        mGroupShapes.push_back(
            new GroupShape(1.f, sf::Vector2f(10.f, 10.f), sf::Color(rand() % 255, rand() % 255, rand() % 255))
        );
    }
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
        if (group_id >= 0 & group_id < mGroupShapes.size()) {
            GroupShape* group_circle = mGroupShapes[group->getId()];

            group_circle->setPosition(group->getPosition());
            group_circle->setRadius(group->getSize());
            group_circle->setActive(true);
        }
    }
}
