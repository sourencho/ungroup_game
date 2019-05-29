#include <stdio.h>
#include <iostream>
#include "Group.hpp"

Group::Group(int id, sf::Vector2f position) {
    mId = id;
    mPosition = position;
    mSize = 0;
}

Group::~Group() {
    //dtor
}

void Group::update() {
    for(auto member: mMembers) {
        mPosition = mPosition + member->getDirection();
    }
}

void Group::addMemeber(Player* player) {
    mMembers.push_back(player);
    mSize = mMembers.size() * 10.f;
}

sf::Vector2f Group::getPosition() const {
    return mPosition;
}

float Group::getSize() const {
    return mSize;
}

int Group::getId() const {
    return mId;
}


void Group::move(const sf::Vector2f& offset) {
    mPosition.x += offset.x;
    mPosition.y += offset.y;
}
