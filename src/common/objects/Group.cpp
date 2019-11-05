#include <iostream>
#include <memory>
#include <numeric>
#include <stdio.h>
#include <vector>

#include "../rendering/RenderingDef.hpp"
#include "Group.hpp"

Group::Group(uint32_t id, sf::Vector2f position, sf::Color color,
             std::shared_ptr<PhysicsController> pc, ResourceStore& rs)
    : CircleGameObject(id, position, 0.f, color, pc, rs, 0.f) {
    setShader(RenderingDef::ShaderKey::voronoi);
}

Group::~Group() {}

bool Group::getJoinable() { return m_Joinable; }

void Group::setJoinable(bool joinable) { m_Joinable = joinable; }

void Group::draw(sf::RenderTarget& render_target) {
    if (m_Joinable) {
        setOutlineThickness(1.f);
        setOutlineColor(RenderingDef::JOINABLE_COLOR);
    } else {
        setOutlineThickness(0.f);
    }
    CircleGameObject::draw(render_target);
}

GroupUpdate Group::getUpdate() {
    sf::Vector2f position = getPosition();
    GroupUpdate gu = {
        .group_id = (sf::Uint32)getId(),
        .is_active = isActive(),
        .x_pos = position.x,
        .y_pos = position.y,
        .radius = getRadius(),
        .joinable = getJoinable(),
        .shader_key = (sf::Uint32)m_Shader.key,
    };

    return gu;
}

void Group::applyUpdate(GroupUpdate gu) {
    setActive(gu.is_active);
    setPosition(sf::Vector2f(gu.x_pos, gu.y_pos));
    setRadius(gu.radius);
    setJoinable(gu.joinable);
    setShader((RenderingDef::ShaderKey)gu.shader_key);
}

sf::Packet& operator<<(sf::Packet& packet, const GroupUpdate& group_update) {
    packet << group_update.group_id << group_update.is_active << group_update.x_pos
           << group_update.y_pos << group_update.radius << group_update.joinable
           << group_update.shader_key;

    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, GroupUpdate& group_update) {
    packet >> group_update.group_id >> group_update.is_active >> group_update.x_pos >>
        group_update.y_pos >> group_update.radius >> group_update.joinable >>
        group_update.shader_key;

    return packet;
}
