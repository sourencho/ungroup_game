/**
 * Base User Interface Element: a base class for elements drawn in the GUI.
 * Managed by the GUIController.
 *
 * Sets the position of the element once during construction.
 * Dynamic data needed for the element should be passed in via <UIData> in the update function.
 * Currently all UI elements share the same struct <UIData> to recieve data.
 */

#ifndef BaseUIElement_hpp
#define BaseUIElement_hpp

#include <SFML/Graphics.hpp>

#include "../../common/systems/ResourceController.hpp"
#include "../../common/util/StateDef.hpp"
#include "RenderingDef.hpp"

struct UIData {
    float game_steps_per_second;
    float game_updates_per_second;
    float server_state_per_second;
    float tick_delta_average;
    float behind_game_state_rate;
    float ahead_game_state_rate;
    float no_game_state_rate;
    float interpolate_distance_average;
    float stall_distance_average;
    std::array<uint32_t, RESOURCE_TYPE_COUNT> resources;
    std::array<uint32_t, RESOURCE_TYPE_COUNT> resource_goals;
    GameStatus game_status;
    uint32_t winner_player_id;
    uint32_t tick;
    float tick_duration_ms;
};

class BaseUIElement {
  public:
    BaseUIElement(sf::Vector2u window_size, sf::Vector2f element_size, RenderingDef::Align align,
                  RenderingDef::Padding padding);
    virtual ~BaseUIElement(){};

    virtual void draw(sf::RenderTarget& render_target) = 0;
    virtual void update(const UIData& ui_data) = 0;

  protected:
    sf::Vector2f m_position;
    void setPosition(sf::Vector2u window_size, sf::Vector2f element_size, RenderingDef::Align align,
                     RenderingDef::Padding padding);
};

#endif /* BaseUIElement_hpp */