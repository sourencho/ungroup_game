#include <stdio.h>
#include <iostream>
#include "ClientGroup.hpp"

ClientGroup::ClientGroup(sf::Vector2f position, sf::Color color,
    std::shared_ptr<PhysicsController> pc)
    : CircleGameObject(0, position, 0.f, color, pc) {}

void ClientGroup::setGroupable(bool groupable) {
  mGroupable = groupable;
}

bool ClientGroup::getGroupable() {
  return mGroupable;
}

ClientGroup::~ClientGroup() {}
