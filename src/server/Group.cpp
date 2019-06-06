#include <stdio.h>
#include <iostream>
#include "Group.hpp"

Group::Group(int id, sf::Vector2f position):GroupShape(0.f, position) {
    mId = id;
    mSize = 0;

}

Group::~Group() {
    //dtor
}

void Group::update() {
    if (shouldDeactivate()) {
        setActive(false);
        return;
    }

    // Calculate new velocity
    sf::Vector2f new_velocity = sf::Vector2f(0.f, 0.f);
    for(auto member: mMembers) {
        new_velocity += member->getDirection();
    }
    setVelocity(new_velocity);

    // Update position
    move();
}

bool Group::shouldDeactivate() {
    for(auto member: mMembers) {
        if (member->isActive()) {
            return false;
        }
    }
    return true;
}

void Group::addMember(Player* player) {
    mMembers.push_back(player);
    mSize = mMembers.size();
    setRadius(mSize * 10.f);
}

int Group::getId() const {
    return mId;
}
