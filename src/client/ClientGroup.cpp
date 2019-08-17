#include <stdio.h>
#include <iostream>
#include "ClientGroup.hpp"

ClientGroup::ClientGroup(sf::Vector2f position, sf::Color color)
    : CircleGameObject(0, position, 0.f, color) {}

void ClientGroup::setGroupable(bool groupable) {
  mGroupable = groupable;
}

bool ClientGroup::getGroupable() {
  return mGroupable;
}

ClientGroup::~ClientGroup() {}
