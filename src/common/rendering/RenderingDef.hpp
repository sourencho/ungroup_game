#ifndef RenderingDef_hpp
#define RenderingDef_hpp

#include <SFML/Graphics.hpp>
#include <memory>

namespace RenderingDef {
    const bool USE_SHADERS = true;
    const std::size_t CIRCLE_POINT_COUNT = 60;
    enum ShaderKey { none, noop, voronoi };
    struct Shader {
        ShaderKey key = ShaderKey::none;
        std::shared_ptr<sf::Shader> shader = nullptr;
    };
    const sf::Color JOINABLE_COLOR(227, 102, 68);
    const sf::Color DARKEST_COLOR(66, 118, 118);
    const sf::Color DARK_COLOR(63, 157, 130);
    const sf::Color LIGHT_COLOR(161, 205, 115);
    const sf::Color LIGHTEST_COLOR(236, 219, 96);
}; // namespace RenderingDef

#endif /* RenderingDef_hpp */