#include "DrawableGroup.hpp"

#include <numeric>

#include "../../common/util/game_settings.hpp"
#include "RenderingDef.hpp"

DrawableGroup::DrawableGroup(ResourceStore& rs) :
    m_resourceStore(rs), m_directionArrow(), m_directionLines() {
    m_outlineShape.setFillColor(sf::Color::Transparent);
    m_circleShape.setFillColor(sf::Color::Blue);
    m_shaderClock.restart();
    setShader(RenderingDef::ShaderKey::voronoi_counts);
}

void DrawableGroup::draw(sf::RenderTarget& target, Group& group, bool joinable, bool ungroup,
                         std::vector<sf::Vector2f> player_directions,
                         std::vector<ResourceType> player_intents,
                         std::array<uint32_t, RESOURCE_TYPE_COUNT> resource_counts) {
    if (!group.isActive()) {
        return;
    }

    if (player_intents.size() != player_directions.size()) {
        throw std::runtime_error("Size of player directions and intents should be the same.");
    }

    size_t player_count = player_directions.size();

    // Draw direction lines
    std::vector<std::pair<sf::Vector2f, sf::Color>> direction_color_pairs;
    direction_color_pairs.reserve(player_count);
    for (size_t i = 0; i < player_count; i++) {
        direction_color_pairs.push_back(
            std::make_pair(player_directions[i], RenderingDef::RESOURCE_COLORS[player_intents[i]]));
    }
    if (SHOW_DIRECTION_LINES) {
        m_directionLines.draw(target, group.getRadius(), group.getPosition(),
                              direction_color_pairs);
    }

    // Draw direction arrows
    if (SHOW_DIRECTION_ARROWS) {
        m_directionArrow.draw(target, group.getRadius(), group.getPosition(), group.getVelocity(),
                              player_directions, RenderingDef::DIRECTION_ARROW_COLOR);
    }

    // Draw group
    m_circleShape.setPosition(group.getPosition());
    m_circleShape.setRadius(group.getRadius());

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

    m_outlineShape.setOutlineColor(outline_color);

    if (m_shader.shader != nullptr && RenderingDef::USE_SHADERS) {
        target.draw(m_circleShape, m_shader.shader.get());
    } else {
        target.draw(m_circleShape);
    }
    target.draw(m_outlineShape);
}

void DrawableGroup::setShader(RenderingDef::ShaderKey shader_key) {
    m_shader.key = shader_key;
    m_shader.shader = m_resourceStore.getShader(shader_key);
}

void DrawableGroup::setTexture(RenderingDef::TextureKey texture_key) {
    m_circleShape.setTexture(m_resourceStore.getTexture(texture_key).get());
}
