/**
 * Renders information for debugging purposes.
 */

#ifndef DebugUIElement_hpp
#define DebugUIElement_hpp

#include <SFML/Graphics.hpp>

#include "../resources/ResourceStore.hpp"
#include "BaseUIElement.hpp"

class DebugUIElement : public BaseUIElement {
  public:
    DebugUIElement(sf::Vector2u window_size, sf::Vector2f size, RenderingDef::Align align,
                   RenderingDef::Padding padding, ResourceStore& rs);
    ~DebugUIElement(){};

    void update(const UIData& ui_data) override;
    void draw(sf::RenderTarget& render_target) override;

  private:
    sf::Text m_text;
    sf::Vector2u m_windowSize;
    RenderingDef::Align m_align;
    RenderingDef::Padding m_padding;
};

#endif /* DebugUIElement_hpp */