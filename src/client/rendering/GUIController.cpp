#include "GUIController.hpp"

#include "FrameRateUIElement.hpp"
#include "TestUIElement.hpp"

GUIController::GUIController(sf::Vector2u window_size, ResourceStore& rs) : m_resourceStore(rs) {
    load(window_size);
}

void GUIController::load(sf::Vector2u window_size) {
    // Test UI element
    // add(std::unique_ptr<TestUIElement>(new TestUIElement(
    //     window_size, {100.f, 200.f}, Align::TOP_LEFT, (Padding){50.f, 50.f, 50.f, 50.f})));
    add(std::unique_ptr<FrameRateUIElement>(
        new FrameRateUIElement(window_size, {0.f, 0.f}, Align::BOTTOM_RIGHT,
                               (Padding){70.f, 0.f, 0.f, 60.f}, m_resourceStore)));
}

void GUIController::add(std::unique_ptr<BaseUIElement> ui_element) {
    m_elements.push_back(std::move(ui_element));
}

void GUIController::update(const UIData& ui_data) {
    for (auto&& ui_element : m_elements) {
        ui_element->update(ui_data);
    }
}

void GUIController::draw(sf::RenderTarget& render_target) {
    for (auto&& ui_element : m_elements) {
        ui_element->draw(render_target);
    }
}