#include "RenderingUtil.hpp"

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