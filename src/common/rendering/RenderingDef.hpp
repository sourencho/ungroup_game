#ifndef RenderingDef_hpp
#define RenderingDef_hpp

#include <SFML/Graphics.hpp>
#include <memory>

namespace RenderingDef {
    const bool USE_SHADERS = true;
    enum ShaderKey { none, noop, voronoi };
    struct Shader {
        ShaderKey key = ShaderKey::none;
        std::shared_ptr<sf::Shader> shader = nullptr;
    };
}; // namespace RenderingDef

#endif /* RenderingDef_hpp */