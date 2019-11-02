#ifndef Circle_hpp
#define Circle_hpp

#include <SFML/Graphics.hpp>
#include <memory>
#include <stdio.h>

class Circle {
  public:
    Circle(float size, sf::Vector2f position, sf::Color color);
    ~Circle();

    void draw(sf::RenderTarget& target);
    void draw(sf::RenderTarget& target, sf::Shader& shader);

    sf::Vector2f getCenter() const;
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f position);

    float getRadius() const;
    void setRadius(int size);
    void setColor();
    void setColor(sf::Color color);
    void changeColor(sf::Color color);
    void setTexture(std::shared_ptr<sf::Texture> texture);

  private:
    sf::CircleShape mCircleShape;
    sf::Color mColor;
};
#endif /* Circle_hpp */
