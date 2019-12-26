#include "DrawableGroup.hpp"

#include <numeric>

#include "../../common/util/game_settings.hpp"
#include "RenderingDef.hpp"

DrawableGroup::DrawableGroup(ResourceStore& rs) :
    DrawableCircle(rs), m_directionArrow(), m_directionLines() {
    setShader(RenderingDef::ShaderKey::voronoi_counts);
}

void DrawableGroup::draw(sf::RenderTarget& target, Group& group, bool joinable, bool ungroup,
                         const std::vector<sf::Vector2f>& player_directions,
                         const std::vector<ResourceType>& player_intents,
                         const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts) {
    if (!group.isActive()) {
        return;
    }

    if (player_intents.size() != player_directions.size()) {
        throw std::runtime_error("Size of player directions and intents should be the same.");
    }

    if (SHOW_DIRECTION_LINES) {
        drawDirectionLines(target, group, player_directions, player_intents);
    }

    if (SHOW_DIRECTION_ARROWS) {
        drawDirectionArrows(target, group, player_directions);
    }

    drawGroup(target, group, joinable, ungroup, resource_counts);
}

void DrawableGroup::drawDirectionLines(sf::RenderTarget& target, Group& group,
                                       const std::vector<sf::Vector2f>& player_directions,
                                       const std::vector<ResourceType>& player_intents) {
    size_t player_count = player_directions.size();
    std::vector<std::pair<sf::Vector2f, sf::Color>> direction_color_pairs;
    direction_color_pairs.reserve(player_count);
    for (size_t i = 0; i < player_count; i++) {
        direction_color_pairs.push_back(
            std::make_pair(player_directions[i], RenderingDef::RESOURCE_COLORS[player_intents[i]]));
    }
    m_directionLines.draw(target, group.getRadius(), group.getPosition(), direction_color_pairs);
}

void DrawableGroup::drawDirectionArrows(sf::RenderTarget& target, Group& group,
                                        const std::vector<sf::Vector2f>& player_directions) {
    m_directionArrow.draw(target, group.getRadius(), group.getPosition(), group.getVelocity(),
                          player_directions, RenderingDef::DIRECTION_ARROW_COLOR);
}

void DrawableGroup::drawGroup(sf::RenderTarget& target, Group& group, bool joinable, bool ungroup,
                              const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts) {

    // Circle
    m_circleShape.setPosition(group.getPosition());
    m_circleShape.setRadius(group.getRadius());

    if (m_shader.shader != nullptr && RenderingDef::USE_SHADERS) {
        std::copy(resource_counts.begin(), resource_counts.end(), m_resourceCounts);
        int total_resource_count =
            std::accumulate(resource_counts.begin(), resource_counts.end(), 0);
        m_shader.shader->setUniform("u_resolution", sf::Vector2f(WINDOW_RESOLUTION));
        m_shader.shader->setUniform("u_position", group.getPosition());
        m_shader.shader->setUniform("u_radius", group.getRadius());
        m_shader.shader->setUniform("u_time", m_shaderClock.getElapsedTime().asSeconds());
        m_shader.shader->setUniformArray("u_resourceCounts", m_resourceCounts, RESOURCE_TYPE_COUNT);
        m_shader.shader->setUniform("u_maxResources", total_resource_count);
    }

    if (m_shader.shader != nullptr && RenderingDef::USE_SHADERS) {
        target.draw(m_circleShape, m_shader.shader.get());
    } else {
        target.draw(m_circleShape);
    }

    // Outline
    m_outlineShape.setPosition(group.getPosition());
    m_outlineShape.setRadius(group.getRadius());
    m_outlineShape.setOutlineThickness(1.f);
    sf::Color outline_color = RenderingDef::DEFAULT_GROUP_OUTLINE_COLOR;

    if (joinable) {
        outline_color = RenderingDef::JOINABLE_COLOR;
    }

    // TODO(sourenp): This was only included for debugging purposes. Remove eventually.
    if (ungroup) {
        outline_color = RenderingDef::UNGROUP_COLOR;
    }

    m_outlineShape.setOutlineColor(outline_color);
    target.draw(m_outlineShape);
}