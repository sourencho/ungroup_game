/**
 * Graphical User Interface Controller.
 * Draws information on top of the game for a client.
 */

#ifndef GUIController_hpp
#define GUIController_hpp

#include <SFML/Graphics.hpp>

#include "BaseUIElement.hpp"

class GUIController {
  public:
    GUIController(sf::Vector2u window_size);
    ~GUIController(){};

    void update();
    void draw(sf::RenderTarget& render_target);

  private:
    void add(std::unique_ptr<BaseUIElement> ui_element);
    void loadLevel(sf::Vector2u window_size);

    std::vector<std::unique_ptr<BaseUIElement>> m_elements;
};

#endif /* GUIController_hpp */