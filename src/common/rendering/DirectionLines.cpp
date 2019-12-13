#include "DirectionLines.hpp"

#include <iostream>

#include "../rendering/RenderingDef.hpp"
#include "../rendering/RenderingUtil.hpp"
#include "../util/game_settings.hpp"

void DirectionLines::draw(
    sf::RenderTarget& render_target, float radius, sf::Vector2f position,
    const std::vector<std::pair<sf::Vector2f, sf::Color>>& direction_color_pairs, bool active) {
    if (!active) {
        return;
    }

    // Create a map of direction to list of colors
    m_directionToColors.clear();
    for (auto& direction_and_color : direction_color_pairs) {
        const auto& direction = direction_and_color.first;
        auto color = direction_and_color.second;
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
