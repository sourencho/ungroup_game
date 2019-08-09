#include <stdio.h>
#include <iostream>
#include "ClientGroup.hpp"

ClientGroup::ClientGroup(sf::Vector2f position):CircleGameObject(0, position, 0.f) {}

void ClientGroup::setGroupable(bool groupable) {
  mGroupable = groupable;
}

bool ClientGroup::getGroupable() {
  return mGroupable;
}

ClientGroup::~ClientGroup() {}
