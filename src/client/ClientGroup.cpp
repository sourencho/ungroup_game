#include <stdio.h>
#include <iostream>
#include "ClientGroup.hpp"


ClientGroup::ClientGroup(sf::Vector2f position, sf::Color color,
  std::shared_ptr<PhysicsController> pc):
  CircleGameObject(0, position, 0.f, color, pc) {}

ClientGroup::~ClientGroup() {}

void ClientGroup::setGroupable(bool groupable) {
  mGroupable = groupable;
}

bool ClientGroup::getGroupable() {
  return mGroupable;
}

void ClientGroup::applyUpdate(GroupUpdate gu) {
    setActive(gu.is_active);
    if (gu.is_active) {
        std::cout << "active group" << std::endl;
    }
    setPosition(sf::Vector2f(gu.x_pos, gu.y_pos));
    setRadius(gu.radius);
    setGroupable(gu.groupable);
}
