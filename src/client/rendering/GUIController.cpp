#include "GUIController.hpp"

#include "DebugUIElement.hpp"
#include "ResourceUIElement.hpp"
#include "TestUIElement.hpp"

const RenderingDef::Padding RESOURCE_UI_PADDING({
    .right = 65.f,
    .left = 0.f,
    .top = 30.f,
    .bottom = 0.f,
});

GUIController::GUIController(sf::Vector2u window_size, ResourceStore& rs) : m_resourceStore(rs) {
    load(window_size);
}

void GUIController::load(sf::Vector2u window_size) {
    // Test UI element
    // add(std::unique_ptr<TestUIElement>(new TestUIElement(
    //     window_size, {100.f, 200.f}, RenderingDef::Align::TOP_LEFT,
    //     (RenderingDef::Padding){50.f, 50.f, 50.f, 50.f})));
    if (RenderingDef::SHOW_DEBUG_UI_ELEMENT) {
        add(std::unique_ptr<DebugUIElement>(
            new DebugUIElement(window_size, {0.f, 0.f}, RenderingDef::Align::BOTTOM_LEFT,
                               RenderingDef::DEBUG_UI_PADDING, m_resourceStore)));
    }
    if (RenderingDef::SHOW_RESOURCE_UI_ELEMENT) {
        add(std::unique_ptr<ResourceUIElement>(
            new ResourceUIElement(window_size, {0.f, 0.f}, RenderingDef::Align::TOP_RIGHT,
                                  RESOURCE_UI_PADDING, m_resourceStore)));
    }
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