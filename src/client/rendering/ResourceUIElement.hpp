/**
 * Renders the client's player's resource counts.
 */

#ifndef ResourceUIElement_hpp
#define ResourceUIElement_hpp

#include <SFML/Graphics.hpp>

#include "../resources/ResourceStore.hpp"
#include "BaseUIElement.hpp"

class ResourceUIElement : public BaseUIElement {
  public:
    ResourceUIElement(sf::Vector2u window_size, sf::Vector2f size, Align align, Padding padding,
                      ResourceStore& rs);
    ~ResourceUIElement(){};

    void update(const UIData& ui_data) override;
    void draw(sf::RenderTarget& render_target) override;

  private:
    std::string getResourceCountString(ResourceType resource_type, uint32_t resource_count);

    sf::Text m_text;
    sf::Vector2u m_windowSize;
    Align m_align;
    Padding m_padding;
};

#endif /* ResourceUIElement_hpp */