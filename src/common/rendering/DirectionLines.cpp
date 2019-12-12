#include "DirectionLines.hpp"

#include <iostream>

#include "../rendering/RenderingDef.hpp"
#include "../rendering/RenderingUtil.hpp"
#include "../util/game_settings.hpp"

DirectionLines::DirectionLines() {
    m_lines.reserve(MAX_PLAYER_COUNT);
    for (size_t i = 0; i < MAX_PLAYER_COUNT; i++) {
        m_lines.push_back(sf::VertexArray(sf::LineStrip));
    }
}

void DirectionLines::draw(
    sf::RenderTarget& render_target, float radius, sf::Vector2f position,
    const std::vector<std::pair<sf::Vector2f, sf::Color>>& direction_color_pairs, bool active) {
    if (!active) {
        return;
    }

    // Create a map of direction to list of colors
    m_directionToColors.clear();
    sf::Vector2f direction;
    sf::Color color;
    for (auto& direction_and_color : direction_color_pairs) {
        std::tie(direction, color) = direction_and_color;
        if (direction == sf::Vector2f(0.f, 0.f)) {
            continue;
        }
        color.a = RenderingDef::DIRECTION_LINE_COLOR_ALPHA;
        if (m_directionToColors.count(direction) == 0) {
            m_directionToColors[direction] = {color};
        } else {
            m_directionToColors[direction].push_back(color);
        }
    }

    // Draw each direction line as a stripped line
    const sf::Vector2f circle_center = position + sf::Vector2f(radius, radius);
    for (auto const& direction_colors : m_directionToColors) {
        const auto& direction = direction_colors.first;
        const auto& colors = direction_colors.second;
        sf::Vector2f circle_edge = circle_center + direction * radius;
        auto line = RenderingUtil::strippedLine(
            circle_edge + (RenderingDef::DIRECTION_LINE_DISTANCE_FROM_EDGE * direction), direction,
            RenderingDef::DIRECTION_LINE_STRIP_LENGTH, colors);
        render_target.draw(line);
    }
}
