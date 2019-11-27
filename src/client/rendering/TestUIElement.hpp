/**
 * Renders a green rectangle used for testing the positioning of UI elements.
 */

#ifndef TestUIElement_hpp
#define TestUIElement_hpp

#include <SFML/Graphics.hpp>

#include "BaseUIElement.hpp"

class TestUIElement : public BaseUIElement {
  public:
    TestUIElement(sf::Vector2u window_size, sf::Vector2f size, Align align, Padding padding);
    ~TestUIElement(){};

    void update(const UIData& ui_data) override;
    void draw(sf::RenderTarget& render_target) override;

  private:
    sf::RectangleShape m_rectangle;
};

#endif /* TestUIElement_hpp */