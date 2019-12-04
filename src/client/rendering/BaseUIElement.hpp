/**
 * Base User Interface Element: a base class for elements drawn in the GUI.
 * Managed by the GUIController.
 *
 * Sets the position of the element once during construction.
 * Dynamic data needed for the element should be passed in via <UIData> in the update function.
 * Currently all UI elements share the same struct <UIData> to recieve data.
 */

#ifndef BaseUIElement_hpp
#define BaseUIElement_hpp

#include <SFML/Graphics.hpp>

#include "../../common/systems/ResourceController.hpp"

enum Align { TOP_RIGHT, TOP_LEFT, BOTTOM_RIGHT, BOTTOM_LEFT };
struct Padding {
    float right;
    float left;
    float top;
    float bottom;
};
struct UIData {
    float steps_per_second;
    float updates_per_second;
    std::array<uint32_t, RESOURCE_TYPE_COUNT> resources;
};

class BaseUIElement {
  public:
    BaseUIElement(sf::Vector2u window_size, sf::Vector2f element_size, Align align,
                  Padding padding);
    virtual ~BaseUIElement(){};

    virtual void draw(sf::RenderTarget& render_target) = 0;
    virtual void update(const UIData& ui_data) = 0;

  protected:
    sf::Vector2f m_position;
    void setPosition(sf::Vector2u window_size, sf::Vector2f element_size, Align align,
                     Padding padding);
};

#endif /* BaseUIElement_hpp */