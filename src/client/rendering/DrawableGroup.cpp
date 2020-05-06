#include "DrawableGroup.hpp"

#include <numeric>

#include "../../common/util/game_settings.hpp"
#include "RenderingDef.hpp"

DrawableGroup::DrawableGroup(ResourceStore& rs) :
    DrawableCircle(rs), m_directionArrow(), m_directionLines() {
    setShader(RenderingDef::ShaderKey::voronoi_counts);
    m_circleShape.setFillColor(RenderingDef::DEFAULT_GROUP_COLOR);
}

void DrawableGroup::draw(sf::RenderTarget& target, Group& group, bool joinable, bool ungroup,
                         const std::vector<std::shared_ptr<Player>>& players,
                         const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts,
                         uint32_t player_id) {
    if (!group.isActive()) {
        return;
    }

    if (RenderingDef::SHOW_DIRECTION_LINES) {
        drawDirectionLines(target, group, players);
    }

    if (RenderingDef::SHOW_DIRECTION_ARROWS) {
        drawDirectionArrows(target, group, players, player_id);
    }

    drawGroup(target, group, joinable, ungroup, resource_counts);
}

void DrawableGroup::drawDirectionLines(sf::RenderTarget& target, Group& group,
                                       const std::vector<std::shared_ptr<Player>>& players) {
    std::vector<std::pair<sf::Vector2f, sf::Color>> direction_color_pairs;
    direction_color_pairs.reserve(players.size());
    std::transform(players.begin(), players.end(), std::back_inserter(direction_color_pairs),
                   [](std::shared_ptr<Player> player) {
                       return std::make_pair(player->getDirection(),
                                             RenderingDef::RESOURCE_COLORS[player->getIntent()]);
                   });
    m_directionLines.draw(target, group.getRadius(), group.getPosition(), direction_color_pairs);
}

void DrawableGroup::drawDirectionArrows(sf::RenderTarget& target, Group& group,
                                        const std::vector<std::shared_ptr<Player>>& players,
                                        uint32_t player_id) {
    std::vector<std::pair<sf::Vector2f, sf::Color>> direction_color_pairs;
    direction_color_pairs.reserve(players.size());

    std::transform(players.begin(), players.end(), std::back_inserter(direction_color_pairs),
                   [player_id](std::shared_ptr<Player> player) {
                       sf::Color player_color = RenderingDef::RESOURCE_COLORS[player->getIntent()];
                       if (player->getId() != player_id) {
                           player_color.a = RenderingDef::NON_PLAYER_DIRECTION_ARROW_ALPHA * 255;
                       }
                       return std::make_pair(player->getDirection(), player_color);
                   });

    m_directionArrow.draw(target, group.getRadius(), group.getPosition(), group.getVelocity(),
                          direction_color_pairs);
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
        m_shader.shader->setUniform("u_resolution", sf::Vector2f(RenderingDef::WINDOW_RESOLUTION));
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
    m_outlineShape.setRadius(group.getRadius() + RenderingDef::GROUP_OUTLINE_DISTANCE);
    m_outlineShape.setPosition(
        group.getCenter() - sf::Vector2f(m_outlineShape.getRadius(), m_outlineShape.getRadius()));
    m_outlineShape.setOutlineThickness(RenderingDef::GROUP_OUTLINE_THICKNESS);
    sf::Color outline_color = RenderingDef::DEFAULT_GROUP_OUTLINE_COLOR;

    if (joinable) {
        m_outlineShape.setOutlineThickness(RenderingDef::GROUP_JOINABLE_THICKNESS);
        outline_color = RenderingDef::JOINABLE_COLOR;
    }

    // TODO(sourenp): This was only included for debugging purposes. Remove eventually.
    if (ungroup) {
        m_outlineShape.setOutlineThickness(RenderingDef::GROUP_JOINABLE_THICKNESS);
        outline_color = RenderingDef::UNGROUP_COLOR;
    }

    m_outlineShape.setOutlineColor(outline_color);
    target.draw(m_outlineShape);
}