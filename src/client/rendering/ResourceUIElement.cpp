#include "ResourceUIElement.hpp"

#include <iomanip>
#include <sstream>

#include "RenderingDef.hpp"

// Pad count with this many zeroes. If a count has more digits than this then things will look
// misaligned.
const int FILL_DIGIT_COUNT = 3;

ResourceUIElement::ResourceUIElement(sf::Vector2u window_size, sf::Vector2f size, Align align,
                                     Padding padding, ResourceStore& rs) :
    BaseUIElement(window_size, size, align, padding),
    m_windowSize(window_size), m_align(align), m_padding(padding) {
    m_text.setFont(*rs.getFont(RenderingDef::FontKey::monogram));
    m_text.setString("NO DATA");
    m_text.setCharacterSize(RenderingDef::RESOURCE_UI_TEXT_SIZE);
    m_text.setFillColor(RenderingDef::RESOURCE_UI_TEXT_COLOR);
    m_text.setPosition(m_position);
}

void ResourceUIElement::update(const UIData& ui_data) {
    std::string resource_str =
        getResourceCountString(ResourceType::RED, ui_data.resources[ResourceType::RED]) + "\n" +
        getResourceCountString(ResourceType::GREEN, ui_data.resources[ResourceType::GREEN]) + "\n" +
        getResourceCountString(ResourceType::BLUE, ui_data.resources[ResourceType::BLUE]) + "\n" +
        getResourceCountString(ResourceType::YELLOW, ui_data.resources[ResourceType::YELLOW]);
    m_text.setString(resource_str);

    // Update position according to text size
    // This doesn't seem to be aligning perfectly and I can't figure out why.
    // For now we can manually adjust it with padding to fix.
    sf::FloatRect bounds = m_text.getLocalBounds();
    BaseUIElement::setPosition(
        m_windowSize, {bounds.left + bounds.width, bounds.height + bounds.top}, m_align, m_padding);
    m_text.setPosition(m_position);
}

void ResourceUIElement::draw(sf::RenderTarget& render_target) {
    render_target.draw(m_text);
}

std::string ResourceUIElement::getResourceCountString(ResourceType resource_type,
                                                      uint32_t resource_count) {
    std::stringstream stream;
    stream << RESOURCE_NAME[resource_type] << " " << std::setw(FILL_DIGIT_COUNT)
           << std::setfill('0') << resource_count;
    return stream.str();
}