/**
 * Draws lines around the edge of a circle. The circle itself is not drawn.
 * The lines represent the direction in which a player within a group wants to go and are colored in
 * the player's desired resource.
 * If several players point in the same direction then the line will be longer and have many colors.
 */

#ifndef DirectionLines_hpp
#define DirectionLines_hpp

#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "../../common/physics/VectorUtil.hpp"

class DirectionLines {
  public:
    DirectionLines(){};
    ~DirectionLines(){};

    /**
     * Draws the target and velocity arrows if active.
     *
     * @radius: size of circle
     * @position: position of circle
     * @directions: list of directions
     */
    void draw(sf::RenderTarget& render_target, float radius, sf::Vector2f position,
              const std::vector<std::pair<sf::Vector2f, sf::Color>>& direction_color_pairs);

  private:
    std::map<sf::Vector2f, std::vector<sf::Color>, VectorUtil::vector_comparator>
        m_directionToColors;
};

#endif /* DirectionLines_hpp */