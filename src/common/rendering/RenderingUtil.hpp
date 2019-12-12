#ifndef RenderingtUtil_hpp
#define RenderingtUtil_hpp

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

namespace RenderingUtil {
    /**
     * Converts a list of ids to a string.
     */
    std::string idVecToStr(std::vector<uint32_t> ids, const std::string& seperator);

    /**
     * Same as SFML's mapCoordToPixel but applies a scaling factor.
     */
    sf::Vector2f mapCoordToPixelScaled(const sf::Vector2f& position, const sf::RenderWindow& window,
                                       const sf::View& view, const sf::Vector2f scaling_factor);

    /**
     * Creates a line that is colored evenly in strips.
     * @start_point: starting point of line.
     * @direction: direction of line, assumed to be a unit vector.
     * @strip_length: length of each individaul strip.
     * @colors: list of colors. The length of the line will be (colors.size() * strip_length).
     */
    sf::VertexArray strippedLine(sf::Vector2f start_point, sf::Vector2f direction,
                                 float strip_length, std::vector<sf::Color> colors);
} // namespace RenderingUtil

#endif /* RenderingtUtil_hpp */