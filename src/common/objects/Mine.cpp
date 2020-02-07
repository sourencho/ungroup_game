#include <iostream>
#include <memory>
#include <numeric>
#include <stdio.h>
#include <vector>

#include "../util/game_settings.hpp"
#include "Mine.hpp"

Mine::Mine(uint32_t id, sf::Vector2f position, float size, ResourceType resource_type,
           PhysicsController& pc) :
    CircleGameObject(id, position, size, pc, std::numeric_limits<float>::infinity(), false),
    m_resourceType(resource_type) {
}

Mine::~Mine() {
}

MineUpdate Mine::getUpdate() {
    sf::Vector2f position = getPosition();
    MineUpdate mu = {
        .mine_id = (sf::Uint32)getId(),
        .is_active = isActive(),
        .x_pos = position.x,
        .y_pos = position.y,
        .radius = getRadius(),
    };
    return mu;
}

void Mine::applyUpdate(MineUpdate mu) {
    setActive(mu.is_active);
    setPosition(sf::Vector2f(mu.x_pos, mu.y_pos));
    setRadius(mu.radius);
}

/* Networking */

sf::Packet& operator<<(sf::Packet& packet, const MineUpdate& mine_update) {
    return packet << mine_update.mine_id << mine_update.is_active << mine_update.x_pos
                  << mine_update.y_pos << mine_update.radius;
}

sf::Packet& operator>>(sf::Packet& packet, MineUpdate& mine_update) {
    return packet >> mine_update.mine_id >> mine_update.is_active >> mine_update.x_pos >>
           mine_update.y_pos >> mine_update.radius;
}
