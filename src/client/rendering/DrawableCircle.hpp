/**
 * Renders a mine
 */

#ifndef DrawableCircle_hpp
#define DrawableCircle_hpp

#include <SFML/Graphics.hpp>

#include "../../common/objects/Mine.hpp"
#include "../resources/ResourceStore.hpp"

class DrawableCircle {
  public:
    explicit DrawableCircle(ResourceStore& rs);
    ~DrawableCircle(){};
    DrawableCircle(const DrawableCircle& temp_obj) = delete;
    DrawableCircle& operator=(const DrawableCircle& temp_obj) = delete;

    void setShader(RenderingDef::ShaderKey shader_key);
    void setTexture(RenderingDef::TextureKey texture_key);

  protected:
    sf::CircleShape m_circleShape;
    sf::CircleShape
        m_outlineShape; // We need another circle hidden behind to draw the outline on. We can't
                        // use the original circle because the shader draw over the outline.

    ResourceStore& m_resourceStore;

    RenderingDef::Shader m_shader;
    sf::Clock m_shaderClock;
};

#endif /* DrawableCircle_hpp */
