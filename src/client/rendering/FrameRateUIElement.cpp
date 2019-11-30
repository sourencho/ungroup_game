#include "FrameRateUIElement.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "../../common/rendering/RenderingDef.hpp"

FrameRateUIElement::FrameRateUIElement(sf::Vector2u window_size, sf::Vector2f size, Align align,
                                       Padding padding, ResourceStore& rs) :
    BaseUIElement(window_size, size, align, padding),
    m_windowSize(window_size), m_align(align), m_padding(padding) {
    m_text.setFont(*rs.getFont(RenderingDef::FontKey::monogram));
    m_text.setString("XXX");
    m_text.setCharacterSize(60.f);
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(m_position);
}

void FrameRateUIElement::update(const UIData& ui_data) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << ui_data.frame_rate;
    m_text.setString(stream.str());

    // Update position according to text size
    sf::FloatRect bounds = m_text.getLocalBounds();
    BaseUIElement::setPosition(
        m_windowSize, {bounds.left + bounds.width, bounds.height + bounds.top}, m_align, m_padding);
    m_text.setPosition(m_position);
}

void FrameRateUIElement::draw(sf::RenderTarget& render_target) {
    render_target.draw(m_text);
}