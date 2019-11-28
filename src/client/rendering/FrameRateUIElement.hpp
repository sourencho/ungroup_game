/**
 * Renders the frames FPS of the client.
 */

#ifndef FrameRateUIElement_hpp
#define FrameRateUIElement_hpp

#include <SFML/Graphics.hpp>

#include "../../common/resources/ResourceStore.hpp"
#include "BaseUIElement.hpp"

class FrameRateUIElement : public BaseUIElement {
  public:
    FrameRateUIElement(sf::Vector2u window_size, sf::Vector2f size, Align align, Padding padding,
                       ResourceStore& rs);
    ~FrameRateUIElement(){};

    void update(const UIData& ui_data) override;
    void draw(sf::RenderTarget& render_target) override;

  private:
    sf::Text m_text;
};

#endif /* FrameRateUIElement_hpp */