#ifndef DirectionArrow_hpp
#define DirectionArrow_hpp

#include <SFML/Graphics.hpp>

class DirectionArrow {
  public:
    DirectionArrow();
    ~DirectionArrow(){};

    void draw(sf::RenderTarget& render_target, float size, float radius, sf::Vector2f position,
              sf::Vector2f velocity, sf::Color color, bool active);

  private:
    sf::CircleShape m_triangle;
};

#endif /* DriectionArrow_hpp */