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
    ResourceUIElement(sf::Vector2u window_size, sf::Vector2f size, RenderingDef::Align align,
                      RenderingDef::Padding padding, ResourceStore& rs);
    ~ResourceUIElement(){};

    void update(const UIData& ui_data) override;
    void draw(sf::RenderTarget& render_target) override;

  private:
    std::string getResourceCountString(ResourceType resource_type, uint32_t resource_count,
                                       uint32_t resource_goal_count);

    std::array<sf::Text, RESOURCE_TYPE_COUNT> m_resourceCountTexts;
    std::array<sf::Sprite, RESOURCE_TYPE_COUNT> m_resourceLetterSprites;
    sf::Vector2u m_windowSize;
    RenderingDef::Align m_align;
    RenderingDef::Padding m_padding;
};

#endif /* ResourceUIElement_hpp */