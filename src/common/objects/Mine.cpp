#include <iostream>
#include <memory>
#include <numeric>
#include <stdio.h>
#include <vector>

#include "../rendering/RenderingDef.hpp"
#include "../util/game_settings.hpp"
#include "Mine.hpp"

sf::Packet& operator<<(sf::Packet& packet, const MineUpdate& mine_update) {
    return packet << mine_update.mine_id << mine_update.is_active << mine_update.x_pos
                  << mine_update.y_pos << mine_update.radius << mine_update.shader_key;
}

sf::Packet& operator>>(sf::Packet& packet, MineUpdate& mine_update) {
    return packet >> mine_update.mine_id >> mine_update.is_active >> mine_update.x_pos >>
           mine_update.y_pos >> mine_update.radius >> mine_update.shader_key;
}

Mine::Mine(uint32_t id, sf::Vector2f position, float size, sf::Color color, PhysicsController& pc,
           ResourceStore& rs) :
    CircleGameObject(id, position, size, color, pc, rs, std::numeric_limits<float>::infinity(),
                     false) {
    setTexture("mine_pattern");
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
        .shader_key = (sf::Uint32)m_shader.key,
    };
    return mu;
}

void Mine::applyUpdate(MineUpdate mu) {
    setActive(mu.is_active);
    setPosition(sf::Vector2f(mu.x_pos, mu.y_pos));
    setRadius(mu.radius);
    setShader((RenderingDef::ShaderKey)mu.shader_key);
}
