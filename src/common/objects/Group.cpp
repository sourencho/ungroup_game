#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include <numeric>

#include "Group.hpp"


Group::Group(uint32_t id, sf::Vector2f position, sf::Color color,
    std::shared_ptr<PhysicsController> pc):CircleGameObject(id, position, 0.f, color, pc) {}

Group::~Group() {}

bool Group::getGroupable() {
    return mGroupable;
}

void Group::setGroupable(bool groupable) {
    mGroupable = groupable;
}

GroupUpdate Group::getUpdate() {
    sf::Vector2f position = getCircle()->getPosition();
    GroupUpdate gu = {(sf::Uint32) getId(), isActive(), position.x, position.y,
      getCircle()->getRadius(), getGroupable()};

    return gu;
}

void Group::applyUpdate(GroupUpdate gu) {
    setActive(gu.is_active);
    setPosition(sf::Vector2f(gu.x_pos, gu.y_pos));
    setRadius(gu.radius);
    setGroupable(gu.groupable);
}

sf::Packet& operator <<(sf::Packet& packet, const GroupUpdate& group_update) {
    packet
        << group_update.group_id
        << group_update.is_active
        << group_update.x_pos
        << group_update.y_pos
        << group_update.radius
        << group_update.groupable;

    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, GroupUpdate& group_update) {
    packet
        >> group_update.group_id
        >> group_update.is_active
        >> group_update.x_pos
        >> group_update.y_pos
        >> group_update.radius
        >> group_update.groupable;

    return packet;
}
