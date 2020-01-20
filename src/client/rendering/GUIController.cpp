#include "GUIController.hpp"

#include "DebugUIElement.hpp"
#include "ResourceUIElement.hpp"
#include "TestUIElement.hpp"

GUIController::GUIController(sf::Vector2u window_size, ResourceStore& rs) : m_resourceStore(rs) {
    load(window_size);
}

void GUIController::load(sf::Vector2u window_size) {
    // Test UI element
    // add(std::unique_ptr<TestUIElement>(new TestUIElement(
    //     window_size, {100.f, 200.f}, Align::TOP_LEFT, (Padding){50.f, 50.f, 50.f, 50.f})));
    if (RenderingDef::SHOW_DEBUG_UI_ELEMENT) {
        add(std::unique_ptr<DebugUIElement>(new DebugUIElement(window_size, {0.f, 0.f},
                                                               Align::BOTTOM_LEFT,
                                                               (Padding){
                                                                   .right = 0.f,
                                                                   .left = 24.f,
                                                                   .top = 0.f,
                                                                   .bottom = 24.f,
                                                               },
                                                               m_resourceStore)));
    }
    if (RenderingDef::SHOW_RESOURCE_UI_ELEMENT) {
        add(std::unique_ptr<ResourceUIElement>(new ResourceUIElement(window_size, {0.f, 0.f},
                                                                     Align::TOP_RIGHT,
                                                                     (Padding){
                                                                         .right = 24.f,
                                                                         .left = 0.f,
                                                                         .top = 0.f,
                                                                         .bottom = 0.f,
                                                                     },
                                                                     m_resourceStore)));
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