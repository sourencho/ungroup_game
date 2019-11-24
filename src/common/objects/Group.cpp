#include <iostream>

#include <memory>
#include <numeric>
#include <stdio.h>
#include <vector>

#include "../rendering/RenderingDef.hpp"
#include "Group.hpp"

Group::Group(uint32_t id, sf::Vector2f position, sf::Color color, PhysicsController& pc,
             ResourceStore& rs) :
    CircleGameObject(id, position, 0.f, color, pc, rs, 0.f),
    m_directionArrow() {
    setShader(RenderingDef::ShaderKey::none);
}

Group::~Group() {
}

void Group::draw(sf::RenderTarget& render_target) {
    setOutlineThickness(1.f);

    sf::Color outline_color = sf::Color::White;

    if (m_joinable) {
        outline_color = RenderingDef::JOINABLE_COLOR;
    }
    // TODO(sourenp): This was only included for debugging purposes. Remove eventually.
    if (m_ungroup) {
        outline_color = sf::Color::Blue;
    }

    setOutlineColor(outline_color);

    CircleGameObject::draw(render_target);

    m_directionArrow.draw(render_target, getRadius() * .3f, getRadius(), getPosition(),
                          getVelocity(), outline_color, m_isActive);
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
        .shader_key = (sf::Uint32)m_shader.key,
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

void Group::setActive(bool active) {
    CircleGameObject::setActive(active);
}

void Group::setDirection(sf::Vector2f direction) {
    applyInput(direction); // TODO(sourenp): Clean this up
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
