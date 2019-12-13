#include <iostream>

#include <memory>
#include <numeric>
#include <stdio.h>
#include <vector>

#include "../rendering/RenderingDef.hpp"
#include "../util/game_settings.hpp"
#include "Group.hpp"

Group::Group(uint32_t id, sf::Vector2f position, sf::Color color, PhysicsController& pc,
             ResourceStore& rs) :
    CircleGameObject(id, position, 0.f, color, pc, rs, 0.f),
    m_directionArrow(), m_directionLines() {
    setShader(RenderingDef::ShaderKey::voronoi);
}

Group::~Group() {
}

void Group::draw(sf::RenderTarget& render_target, bool joinable, bool ungroup,
                 std::vector<sf::Vector2f> player_directions,
                 std::vector<ResourceType> player_intents) {

    setOutlineThickness(0.f);
    sf::Color outline_color = RenderingDef::DEFAULT_GROUP_OUTLINE_COLOR;

    if (joinable) {
        setOutlineThickness(1.f);
        outline_color = RenderingDef::JOINABLE_COLOR;
    }
    // TODO(sourenp): This was only included for debugging purposes. Remove eventually.
    if (ungroup) {
        setOutlineThickness(1.f);
        outline_color = RenderingDef::UNGROUP_COLOR;
    }
    setOutlineColor(outline_color);

    if (player_directions.size() != player_intents.size()) {
        throw std::runtime_error("Size of player directions and intents should be the same.");
    }
    std::vector<std::pair<sf::Vector2f, sf::Color>> direction_color_pairs;
    direction_color_pairs.reserve(player_directions.size());
    for (size_t i = 0; i < player_directions.size(); i++) {
        direction_color_pairs.push_back(
            std::make_pair(player_directions[i], RenderingDef::RESOURCE_COLORS[player_intents[i]]));
    }

    if (SHOW_DIRECTION_LINES) {
        m_directionLines.draw(render_target, getRadius(), getPosition(), direction_color_pairs,
                              m_isActive);
    }
    if (SHOW_DIRECTION_ARROWS) {
        m_directionArrow.draw(render_target, getRadius(), getPosition(), getVelocity(),
                              player_directions, RenderingDef::DIRECTION_ARROW_COLOR, m_isActive);
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
        .shader_key = (sf::Uint32)m_shader.key,
    };

    return gu;
}

void Group::applyUpdate(GroupUpdate gu) {
    setActive(gu.is_active);
    setPosition(sf::Vector2f(gu.x_pos, gu.y_pos));
    setRadius(gu.radius);
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
           << group_update.y_pos << group_update.radius << group_update.shader_key;

    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, GroupUpdate& group_update) {
    packet >> group_update.group_id >> group_update.is_active >> group_update.x_pos >>
        group_update.y_pos >> group_update.radius >> group_update.shader_key;

    return packet;
}
