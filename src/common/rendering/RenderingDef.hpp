#ifndef RenderingDef_hpp
#define RenderingDef_hpp

#include <SFML/Graphics.hpp>
#include <memory>

#include "../systems/ResourceController.hpp"

namespace RenderingDef {
    const bool USE_SHADERS = true;
    const std::size_t CIRCLE_POINT_COUNT = 60;
    enum TextureKey { collision, mine_pattern };
    enum ShaderKey { none, noop, voronoi };
    enum FontKey { monogram };
    struct Shader {
        ShaderKey key = ShaderKey::none;
        std::shared_ptr<sf::Shader> shader = nullptr;
    };

    // Group
    const sf::Color DEFAULT_GROUP_COLOR(sf::Color::Transparent);
    const sf::Color DEFAULT_GROUP_OUTLINE_COLOR(sf::Color::Black);
    const sf::Color JOINABLE_COLOR(227, 102, 68);
    const sf::Color UNGROUP_COLOR(sf::Color::Blue);
    const sf::Color DIRECTION_ARROW_COLOR(63, 154, 233);
    const sf::Color PLAYER_ID_TEXT_COLOR(sf::Color::Black);
    const float PLAYER_ID_TEXT_SIZE(50.f);

    // Resource
    const sf::Color DARKEST_COLOR(66, 118, 118);
    const sf::Color DARK_COLOR(63, 157, 130);
    const sf::Color LIGHT_COLOR(161, 205, 115);
    const sf::Color LIGHTEST_COLOR(236, 219, 96);

    // Mine
    const sf::Color DEFAULT_MINE_COLOR(sf::Color::Red);
    const sf::Color EMPTY_MINE_COLOR(155, 155, 155);
    const std::array<sf::Color, RESOURCE_TYPE_COUNT>
        MINE_COLORS({RenderingDef::DARKEST_COLOR, RenderingDef::DARK_COLOR,
                     RenderingDef::LIGHT_COLOR, RenderingDef::LIGHTEST_COLOR});

    // Misc
    const sf::Color BACKGROUND_COLOR(203, 219, 252);
    const sf::Color COLLISION_ANIMATION_COLOR(sf::Color::Yellow);

    // UI
    const sf::Color RESOURCE_UI_TEXT_COLOR(sf::Color::Black);
    const float RESOURCE_UI_TEXT_SIZE(60.f);
    const sf::Color DEBUG_UI_TEXT_COLOR(sf::Color::Blue);
    const float DEBUG_UI_TEXT_SIZE(50.f);

}; // namespace RenderingDef

#endif /* RenderingDef_hpp */