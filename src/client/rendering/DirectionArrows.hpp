/**
 * Draws arrows around the edge of a circle. The circle itself is not drawn.
 *
 * This class is specifically written for a circle with a velocity and target directions so it's
 * optimized to draw those. It will draw one velocity arrow and many others, in a lighter shade,
 * showing target directions.
 */

#ifndef DirectionArrows_hpp
#define DirectionArrows_hpp

#include <SFML/Graphics.hpp>

class DirectionArrows {
  public:
    DirectionArrows();
    ~DirectionArrows(){};

    /**
     * Draws the target and velocity arrows if active.
     *
     * @radius: size of circle
     * @position: position of circle
     * @velocity: velocity of circle
     * @targets: list of target directions
     * @color: color of arrows
     */
    void draw(sf::RenderTarget& render_target, float radius, sf::Vector2f position,
              sf::Vector2f velocity, const std::vector<sf::Vector2f>& targets, sf::Color color);

  private:
    /**
     * Draw a single arrow in the velocity direction as long as both x and y are larger than
     * MIN_SPEED.
     */
    void drawVelocityArrow(sf::RenderTarget& render_target, float radius, sf::Vector2f position,
                           sf::Vector2f velocity, sf::Color color);

    /**
     * Draw arrows in all directions passed in as long as they aren't (0,0).
     */
    void drawTargetArrows(sf::RenderTarget& render_target, float radius, sf::Vector2f position,
                          std::vector<sf::Vector2f> directions, sf::Color color);

    /**
     * Utility function used to position and rotate a triangle on the outside rim of a circle.
     *
     * @triangle: triangle shape to transform
     * @direction: direction to point triangle
     * @size: size of triangle
     * @radius: radius of circle
     */
    void positionTriangle(sf::CircleShape& triangle, sf::Vector2f direction, sf::Vector2f position,
                          float size, float radius);

    sf::CircleShape m_velocityTriangle;
    std::vector<sf::CircleShape> m_targetTriangles;
};

#endif /* DirectionArrows_hpp */