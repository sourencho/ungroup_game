#include "FrameRateUIElement.hpp"

#include <iomanip>
#include <sstream>

#include "../../common/rendering/RenderingDef.hpp"

FrameRateUIElement::FrameRateUIElement(sf::Vector2u window_size, sf::Vector2f size, Align align,
                                       Padding padding, ResourceStore& rs) :
    BaseUIElement(window_size, size, align, padding) {
    m_text.setFont(*rs.getFont(RenderingDef::FontKey::silkscreen));
    m_text.setString("XXX");
    m_text.setCharacterSize(40.f);
    m_text.setFillColor(sf::Color::White);
    m_text.setPosition(m_position);
}

void FrameRateUIElement::update(const UIData& ui_data) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << ui_data.frame_rate;
    m_text.setString(stream.str());
}

void FrameRateUIElement::draw(sf::RenderTarget& render_target) {
    render_target.draw(m_text);
}