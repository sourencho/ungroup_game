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
} // namespace RenderingUtil

#endif /* RenderingtUtil_hpp */