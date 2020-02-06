#ifndef RenderingDef_hpp
#define RenderingDef_hpp

#include <SFML/Graphics.hpp>
#include <memory>

#include "../../common/systems/ResourceController.hpp"
#include "../../common/util/game_settings.hpp"

namespace RenderingDef {
    const uint WINDOW_FRAME_LIMIT = 60;
    const sf::Vector2f WINDOW_RESOLUTION(1200, 900);
    const float GAME_SCALE = 4.f;
    const sf::Vector2f GAME_SCALING_FACTOR(GAME_SCALE, GAME_SCALE);
    const bool USE_SHADERS = true;
    const std::size_t CIRCLE_POINT_COUNT = 60;
    enum TextureKey {
        collision,
        mine_pattern,
        dotted_background,
        a_letter,
        m_letter,
        e_letter,
        n_letter
    };
    enum ShaderKey { none, noop, voronoi_counts };
    enum FontKey { monogram };
    struct Shader {
        ShaderKey key = ShaderKey::none;
        std::shared_ptr<sf::Shader> shader = nullptr;
    };
    enum Align { TOP_RIGHT, TOP_LEFT, BOTTOM_RIGHT, BOTTOM_LEFT };
    struct Padding {
        float right;
        float left;
        float top;
        float bottom;
    };

    /* Debug */
    const bool RENDER_DEBUG = false;
    const sf::Color DEBUG_TEXT_COLOR(sf::Color::Green);

    /* Group */
    const sf::Color DEFAULT_GROUP_COLOR(255, 255, 255, 0.05 * 255);
    const sf::Color DEFAULT_GROUP_OUTLINE_COLOR(sf::Color::White);
    const sf::Color JOINABLE_COLOR(255, 255, 255, 1.f * 255);
    const sf::Color UNGROUP_COLOR(0, 146, 199, 1.f * 255);
    const float GROUP_OUTLINE_THICKNESS(0.f);
    const float GROUP_OUTLINE_DISTANCE(1.f);
    const float GROUP_JOINABLE_THICKNESS(1.f);

    // Direction Arrow
    const bool SHOW_DIRECTION_ARROWS = true;
    const sf::Color DIRECTION_ARROW_COLOR(sf::Color::White);

    // Direction Line
    const bool SHOW_DIRECTION_LINES = false;
    const sf::Color DIRECTION_LINE_DEFAULT_COLOR(120, 120, 120);
    const float DIRECTION_LINE_STRIP_LENGTH(3.f);
    const float DIRECTION_LINE_COLOR_ALPHA(255 * 1.f);
    const float DIRECTION_LINE_DISTANCE_FROM_EDGE(2.f);

    // Player IDs
    const sf::Color PLAYER_ID_TEXT_COLOR(DEBUG_TEXT_COLOR);
    const float PLAYER_ID_TEXT_SIZE(50.f);
    const bool SHOW_PLAYER_IDS = true && RENDER_DEBUG;

    /* Resource */
    // Green palette
    // const sf::Color RESOURCE_A_COLOR(66, 118, 118);
    // const sf::Color RESOURCE_B_COLOR(63, 157, 130);
    // const sf::Color RESOURCE_C_COLOR(161, 205, 115);
    // const sf::Color RESOURCE_D_COLOR(236, 219, 96);

    // Soft palette
    const sf::Color RESOURCE_A_COLOR(159, 224, 246);
    const sf::Color RESOURCE_B_COLOR(243, 229, 154);
    const sf::Color RESOURCE_C_COLOR(243, 181, 155);
    const sf::Color RESOURCE_D_COLOR(243, 156, 156);

    /* Mine */
    const sf::Color DEFAULT_MINE_COLOR(sf::Color::Red);
    const float MINE_OUTLINE_THICKNESS = 0.f;
    const std::array<sf::Color, RESOURCE_TYPE_COUNT>
        RESOURCE_COLORS({RenderingDef::RESOURCE_A_COLOR, RenderingDef::RESOURCE_B_COLOR,
                         RenderingDef::RESOURCE_C_COLOR, RenderingDef::RESOURCE_D_COLOR});
    const std::array<TextureKey, RESOURCE_TYPE_COUNT> RESOURCE_TEXTURE_KEYS(
        {TextureKey::a_letter, TextureKey::m_letter, TextureKey::e_letter, TextureKey::n_letter});

    // Directions
    const bool SHOW_MINE_DIRECTIONS = true;
    const float MINE_DIRECTION_PADDING = 8.f;

    /* Misc */
    const sf::Color BACKGROUND_COLOR(34, 32, 52);
    const sf::Color OUT_OF_BOUNDS_COLOR(120, 120, 120);
    const size_t GAME_BOUNDS_CIRCLE_SIDES = 80;
    const float BACKGROUND_TEXTURE_1_ALPHA = 255 * .8;
    const float BACKGROUND_TEXTURE_2_ALPHA = 255 * .7;
    const float BACKGROUND_TEXUTURE_1_SPEED =
        1.f / 10.f; // Speed at which background texture 1 moves relative to player movement.
    const float BACKGROUND_TEXUTURE_2_SPEED =
        1.f / 5.f; // // Speed at which background texture 2 moves relative to player movement.
    const sf::Vector2f BACKGROUND_TEXTURE_1_SCALE = {2.f, 2.f};
    const sf::Vector2f BACKGROUND_TEXTURE_2_SCALE = {1.f, 1.f};

    const sf::Color COLLISION_ANIMATION_COLOR(sf::Color::White);

    /* UI */
    // Debug
    const bool SHOW_DEBUG_UI_ELEMENT = true && RENDER_DEBUG;
    const sf::Color DEBUG_UI_TEXT_COLOR(DEBUG_TEXT_COLOR);
    const float DEBUG_UI_TEXT_SIZE(40.f);
    const RenderingDef::Padding DEBUG_UI_PADDING({
        .right = 0.f,
        .left = 24.f,
        .top = 0.f,
        .bottom = 24.f,
    });

    // Resource
    const bool SHOW_RESOURCE_UI_ELEMENT = true;
    const float RESOURCE_UI_TEXT_SIZE(58.f);
    const sf::Color RESOURCE_UI_TEXT_COLOR(sf::Color::White);
    const RenderingDef::Padding RESOURCE_UI_PADDING({
        .right = MINE_DIRECTION_PADDING + 93.f,
        .left = 0.f,
        .top = MINE_DIRECTION_PADDING + 20.f, // Text seems to add padding to the top
        .bottom = 0.f,
    });

    /* Screens */
    const sf::Color CONNECTING_SCREEN_COLOR(RESOURCE_A_COLOR);
    const sf::Color WINNER_SCREEN_COLOR(RESOURCE_D_COLOR);

}; // namespace RenderingDef

#endif /* RenderingDef_hpp */
