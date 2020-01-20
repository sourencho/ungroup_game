#include "TestUIElement.hpp"

#include <iostream>

TestUIElement::TestUIElement(sf::Vector2u window_size, sf::Vector2f size, RenderingDef::Align align,
                             RenderingDef::Padding padding) :
    BaseUIElement(window_size, size, align, padding),
    m_rectangle(size) {
    m_rectangle.setPosition(m_position);
    m_rectangle.setFillColor(sf::Color::Green);
}

void TestUIElement::update(const UIData& ui_data) {
    // noop
}

void TestUIElement::draw(sf::RenderTarget& render_target) {
    render_target.draw(m_rectangle);
}