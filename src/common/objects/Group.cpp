#include <iostream>

#include <memory>
#include <numeric>
#include <stdio.h>
#include <vector>

#include "../util/game_settings.hpp"
#include "Group.hpp"

Group::Group(uint32_t id, sf::Vector2f position, PhysicsController& pc) :
    CircleGameObject(id, position, 0.f, pc, 0.f) {
}

Group::~Group() {
}

GroupUpdate Group::getUpdate() {
    sf::Vector2f position = getPosition();
    GroupUpdate gu = {.group_id = (sf::Uint32)getId(),
                      .is_active = isActive(),
                      .x_pos = position.x,
                      .y_pos = position.y,
                      .radius = getRadius()};

    return gu;
}

void Group::applyUpdate(GroupUpdate gu) {
    setActive(gu.is_active);
    setPosition(sf::Vector2f(gu.x_pos, gu.y_pos));
    setRadius(gu.radius);
}

void Group::applyInterpolatedUpdate(GroupUpdate gu, float a, sf::Int32 delta_ms) {
    setActive(gu.is_active);
    hermiteInterpolatePosition({gu.x_pos, gu.y_pos}, gu.velocity, a, delta_ms);
    setRadius(gu.radius);
}

void Group::setActive(bool active) {
    CircleGameObject::setActive(active);
}

void Group::setDirection(sf::Vector2f direction) {
    applyInput(direction); // TODO(sourenp): Clean this up
}

sf::Packet& operator<<(sf::Packet& packet, const GroupUpdate& group_update) {
    packet << group_update.group_id << group_update.is_active << group_update.x_pos
           << group_update.y_pos << group_update.radius << group_update.velocity.x
           << group_update.velocity.y;

    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, GroupUpdate& group_update) {
    packet >> group_update.group_id >> group_update.is_active >> group_update.x_pos >>
        group_update.y_pos >> group_update.radius;

    float vel_x, vel_y;
    packet >> vel_x >> vel_y;
    group_update.velocity = {vel_x, vel_y};

    return packet;
}
