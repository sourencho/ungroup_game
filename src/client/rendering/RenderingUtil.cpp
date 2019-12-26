#include "RenderingUtil.hpp"

#include "../../common/physics/VectorUtil.hpp"

#include <sstream>

std::string RenderingUtil::idVecToStr(std::vector<uint32_t> ids, const std::string& seperator) {
    std::stringstream ss;
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i != 0)
            ss << seperator;
        ss << ids[i];
    }
    return ss.str();
}

sf::Vector2f RenderingUtil::mapCoordToPixelScaled(const sf::Vector2f& position,
                                                  const sf::RenderWindow& window,
                                                  const sf::View& view,
                                                  const sf::Vector2f scaling_factor) {
    return sf::Vector2f(window.mapCoordsToPixel(
        {position.x * scaling_factor.x, position.y * scaling_factor.y}, view));
}

/**
 * We can't draw one point for each color because the renderer will draw a color gradient between
 * the lines. Instead we draw to points of the same color at the ends of each strip.
 */
sf::VertexArray RenderingUtil::strippedLine(sf::Vector2f start_point, sf::Vector2f direction,
                                            float strip_length, std::vector<sf::Color> colors) {
    sf::VertexArray vertex_array(sf::LineStrip, colors.size() * 2);
    sf::Vector2f strip_start_point, strip_end_point;
    for (size_t c = 0; c < colors.size(); c++) {
        strip_start_point = start_point + direction * strip_length * static_cast<float>(c);
        strip_end_point = strip_start_point + direction * strip_length;
        vertex_array[2 * c] = sf::Vertex(strip_start_point, colors[c]);
        vertex_array[2 * c + 1] = sf::Vertex(strip_end_point, colors[c]);
    }
    return vertex_array;
}