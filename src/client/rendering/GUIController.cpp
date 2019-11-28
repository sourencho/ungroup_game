#include "GUIController.hpp"

#include "TestUIElement.hpp"

GUIController::GUIController(sf::Vector2u window_size) {
    load(window_size);
}

void GUIController::load(sf::Vector2u window_size) {
    // Test UI element
    // add(std::unique_ptr<TestUIElement>(new TestUIElement(
    //     window_size, {100.f, 200.f}, Align::TOP_LEFT, (Padding){50.f, 50.f, 50.f, 50.f})));
}

void GUIController::add(std::unique_ptr<BaseUIElement> ui_element) {
    m_elements.push_back(std::move(ui_element));
}

void GUIController::update() {
    for (auto&& ui_element : m_elements) {
        ui_element->update({});
    }
}

void GUIController::draw(sf::RenderTarget& render_target) {
    for (auto&& ui_element : m_elements) {
        ui_element->draw(render_target);
    }
}