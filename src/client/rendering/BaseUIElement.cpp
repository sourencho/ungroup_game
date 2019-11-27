#include "BaseUIElement.hpp"

BaseUIElement::BaseUIElement(sf::Vector2u window_size, sf::Vector2f size, Align align,
                             Padding padding) :
    m_position({10., 10.}) {
    // We only need to set the position once because the window size is static
    setPosition(window_size, size, align, padding);
}

void BaseUIElement::setPosition(sf::Vector2u window_size, sf::Vector2f element_size, Align align,
                                Padding padding) {

    switch (align) {
        case Align::TOP_LEFT: {
            m_position.x = padding.left;
            m_position.y = padding.top;
            break;
        }
        case Align::TOP_RIGHT: {
            m_position.x = window_size.x - padding.right - element_size.x;
            m_position.y = padding.top;
            break;
        }
        case Align::BOTTOM_LEFT: {
            m_position.x = padding.left;
            m_position.y = window_size.y - padding.bottom - element_size.y;
            break;
        }
        case Align::BOTTOM_RIGHT: {
            m_position.x = window_size.x - padding.right - element_size.x;
            m_position.y = window_size.y - padding.bottom - element_size.y;
            break;
        }
    }
}