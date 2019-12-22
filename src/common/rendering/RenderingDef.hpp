#ifndef RenderingDef_hpp
#define RenderingDef_hpp

#include <SFML/Graphics.hpp>
#include <memory>

#include "../systems/ResourceController.hpp"

namespace RenderingDef {
    const bool USE_SHADERS = true;
    const std::size_t CIRCLE_POINT_COUNT = 60;
    enum TextureKey { collision, mine_pattern, dotted_background };
    enum ShaderKey { none, noop, voronoi, voronoi_counts };
    enum FontKey { monogram };
    struct Shader {
        ShaderKey key = ShaderKey::none;
        std::shared_ptr<sf::Shader> shader = nullptr;
    };

    /* Group */
    const sf::Color DEFAULT_GROUP_COLOR(sf::Color::Transparent);
    const sf::Color DEFAULT_GROUP_OUTLINE_COLOR(200, 200, 200);
    const sf::Color JOINABLE_COLOR(sf::Color::Green);
    const sf::Color UNGROUP_COLOR(sf::Color::Blue);
    const sf::Color PLAYER_ID_TEXT_COLOR(sf::Color::Black);
    const float PLAYER_ID_TEXT_SIZE(50.f);

    /* Direction lines and arrows */
    // Arrow
    const sf::Color DIRECTION_ARROW_COLOR(63, 154, 233);

    // Line
    const sf::Color DIRECTION_LINE_DEFAULT_COLOR(120, 120, 120);
    const float DIRECTION_LINE_STRIP_LENGTH(3.f);
    const float DIRECTION_LINE_COLOR_ALPHA(255 * 1.f);
    const float DIRECTION_LINE_DISTANCE_FROM_EDGE(1.f);

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
    const sf::Color EMPTY_MINE_COLOR(155, 155, 155);
    const std::array<sf::Color, RESOURCE_TYPE_COUNT>
        RESOURCE_COLORS({RenderingDef::RESOURCE_A_COLOR, RenderingDef::RESOURCE_B_COLOR,
                         RenderingDef::RESOURCE_C_COLOR, RenderingDef::RESOURCE_D_COLOR});

    /* Misc */
    const sf::Color BACKGROUND_COLOR(20, 20, 20);
    const sf::Color OUT_OF_BOUNDS_COLOR(100, 100, 100);
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
    const sf::Color RESOURCE_UI_TEXT_COLOR(sf::Color::White);
    const float RESOURCE_UI_TEXT_SIZE(60.f);
    const sf::Color DEBUG_UI_TEXT_COLOR(sf::Color::Green);
    const float DEBUG_UI_TEXT_SIZE(50.f);

}; // namespace RenderingDef

#endif /* RenderingDef_hpp */