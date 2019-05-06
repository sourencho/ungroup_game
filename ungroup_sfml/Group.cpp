#include <stdio.h>
#include <iostream>
#include "Group.hpp"

Group::Group(
    float size,
    sf::Vector2f position,
    sf::Color color
):mCircle(size) {
    mPosition = position;
    mCircle.setFillColor(color);
}

Group::~Group() {
    //dtor
}

void Group::update() {
    for(auto member: mMembers) {
        mPosition = mPosition + member->getDirection();
    }
    mCircle.setPosition(mPosition);
}

void Group::draw(sf::RenderTarget& target) {
    target.draw(mCircle);
}

void Group::handleEvents(sf::Event& event) {
    //
}

void Group::addMemeber(Player* player) {
    mMembers.push_back(player);
}
