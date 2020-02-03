#include "ResourceUIElement.hpp"

#include <iomanip>
#include <sstream>

#include "RenderingDef.hpp"

// Pad count with this many zeroes. If a count has more digits than this then things will look
// misaligned.
const int FILL_DIGIT_COUNT = 2;
const float LETTER_COUNT_PADDING = 16.f;
const float COUNT_ROW_PADDING = 16.f;

ResourceUIElement::ResourceUIElement(sf::Vector2u window_size, sf::Vector2f size,
                                     RenderingDef::Align align, RenderingDef::Padding padding,
                                     ResourceStore& rs) :
    BaseUIElement(window_size, size, align, padding),
    m_windowSize(window_size), m_align(align), m_padding(padding) {

    sf::Vector2f row_position = m_position;
    for (size_t i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        auto& text = m_resourceCountTexts[i];
        text.setFont(*rs.getFont(RenderingDef::FontKey::monogram));
        text.setString("XXX");
        text.setCharacterSize(55.f);
        text.setFillColor(RenderingDef::RESOURCE_UI_TEXT_COLOR);
        sf::FloatRect text_rect = text.getGlobalBounds();
        text.setPosition({row_position.x - text_rect.width, row_position.y});
        text_rect = text.getGlobalBounds();

        auto& sprite = m_resourceLetterSprites[i] =
            sf::Sprite(*rs.getTexture(RenderingDef::RESOURCE_TEXTURE_KEYS[i]).get());
        sprite.setScale({2.5f, 2.5f});
        sprite.setColor(RenderingDef::RESOURCE_COLORS[i]);
        const sf::FloatRect sprite_rect = sprite.getGlobalBounds();
        sprite.setPosition(
            {text_rect.left - sprite_rect.width - LETTER_COUNT_PADDING, text_rect.top});
        row_position.y += text_rect.height + COUNT_ROW_PADDING;
    }
}

void ResourceUIElement::update(const UIData& ui_data) {
    for (size_t i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        m_resourceCountTexts[i].setString(getResourceCountString(
            ResourceType(i), ui_data.resources[i], ui_data.resource_goals[i]));
    }
}

void ResourceUIElement::draw(sf::RenderTarget& render_target) {
    for (size_t i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        render_target.draw(m_resourceLetterSprites[i]);
        render_target.draw(m_resourceCountTexts[i]);
    }
}

std::string ResourceUIElement::getResourceCountString(ResourceType resource_type,
                                                      uint32_t resource_count,
                                                      uint32_t resource_goal_count) {
    std::stringstream stream;
    stream << std::setw(FILL_DIGIT_COUNT) << std::setfill('0') << resource_count;
    stream << "/" << std::setw(FILL_DIGIT_COUNT) << std::setfill('0') << resource_goal_count;
    return stream.str();
}