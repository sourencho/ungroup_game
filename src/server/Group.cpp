#include <stdio.h>
#include <iostream>
#include "Group.hpp"
#include "../common/collision.hpp"

Group::Group(int id, sf::Vector2f position) {
    mId = id;
    mSize = 0;
    mCircle = new Circle(0.f, position);
}

Group::~Group() {
    //dtor
}

void Group::update() {
    if (shouldDeactivate()) {
        setActive(false);
        return;
    }

    // Update velocity
    sf::Vector2f new_velocity = sf::Vector2f(0.f, 0.f);
    for(auto member: mMembers) {
        new_velocity += member->getDirection();
    }
    mCircle->setVelocity(new_velocity);

    // Update position
    mCircle->move();
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
    mCircle->setRadius(mSize * 10.f);
}

int Group::getId() const {
    return mId;
}

Circle* Group::getCircle() {
    return mCircle;
}

void Group::handleCollisions(std::vector<Group*>& groups) {
    std::vector<Circle*> circles;
    for(auto group: groups) {
        circles.push_back(group->getCircle());
    }
    handle_circle_collision(circles);
}
