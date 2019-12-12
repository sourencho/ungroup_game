#include "DirectionLines.hpp"

#include <iostream>

#include "../rendering/RenderingDef.hpp"
#include "../util/game_settings.hpp"

const float LINE_LENGTH_RADIUS_RATIO = 1.f;
const float LINE_COLOR_ALPHA = 255 * 0.5f;
const float DISTANCE_FROM_EDGE = 1.f; // Additional distance from cricle's edge

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

    // Populate direction counts
    m_directionToColors.clear();
    sf::Vector2f direction;
    sf::Color color;
    for (auto& direction_and_color : direction_color_pairs) {
        std::tie(direction, color) = direction_and_color;
        if (direction == sf::Vector2f(0.f, 0.f)) {
            continue;
        }
        if (m_directionToColors.count(direction) == 0) {
            m_directionToColors[direction] = {color};
        } else {
            m_directionToColors[direction].push_back(color);
        }
    }

    const float line_chunk_length =
        radius * LINE_LENGTH_RADIUS_RATIO / direction_color_pairs.size();
    const sf::Vector2f circle_center = position + sf::Vector2f(radius, radius);

    size_t i = 0;
    std::cout << m_directionToColors.size() << std::endl;
    for (auto const& direction_colors : m_directionToColors) {
        const auto& direction = direction_colors.first;
        const auto& colors = direction_colors.second;

        auto& line = m_lines[i];
        line.clear();

        for (size_t c = 0; c < colors.size(); c++) {
            sf::Color color = colors[c];
            color.a = LINE_COLOR_ALPHA;
            sf::Vector2f start_point =
                circle_center + direction * (radius + DISTANCE_FROM_EDGE + (line_chunk_length * c));
            sf::Vector2f end_point = circle_center + direction * (radius + DISTANCE_FROM_EDGE +
                                                                  (line_chunk_length * (c + 1)));
            line.append(sf::Vertex(start_point, color));
            line.append(sf::Vertex(end_point, color));
        }

        render_target.draw(line);
        i++;
    }
}
