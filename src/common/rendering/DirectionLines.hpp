/**
 * TODO
 */

#ifndef DirectionLines_hpp
#define DirectionLines_hpp

#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "../physics/VectorUtil.hpp"

class DirectionLines {
  public:
    DirectionLines();
    ~DirectionLines(){};

    /**
     * Draws the target and velocity arrows if active.
     *
     * @radius: size of circle
     * @position: position of circle
     * @directions: list of directions
     * @active: whether or not to draw the arrows
     */
    void draw(sf::RenderTarget& render_target, float radius, sf::Vector2f position,
              const std::vector<std::pair<sf::Vector2f, sf::Color>>& direction_color_pairs,
              bool active);

  private:
    std::vector<sf::VertexArray> m_lines;
    std::map<sf::Vector2f, std::vector<sf::Color>, VectorUtil::vector_comparator>
        m_directionToColors;
};

#endif /* DirectionLines_hpp */