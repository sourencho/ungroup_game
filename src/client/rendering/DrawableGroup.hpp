/**
 * Renders a group
 */

#ifndef DrawableGroup_hpp
#define DrawableGroup_hpp

#include <SFML/Graphics.hpp>

#include <vector>

#include "../../common/objects/Group.hpp"
#include "../resources/ResourceStore.hpp"
#include "DirectionArrows.hpp"
#include "DirectionLines.hpp"

class DrawableGroup {
  public:
    explicit DrawableGroup(ResourceStore& rs);
    ~DrawableGroup(){};
    DrawableGroup(const DrawableGroup& temp_obj) = delete;
    DrawableGroup& operator=(const DrawableGroup& temp_obj) = delete;

    void draw(sf::RenderTarget& target, Group& group, bool joinable, bool ungroup,
              std::vector<sf::Vector2f> player_directions, std::vector<ResourceType> player_intents,
              std::array<uint32_t, RESOURCE_TYPE_COUNT> resource_counts);

    void setShader(RenderingDef::ShaderKey shader_key);
    void setTexture(RenderingDef::TextureKey texture_key);

  private:
    ResourceStore& m_resourceStore;

    sf::CircleShape m_circleShape;
    sf::CircleShape
        m_outlineShape; // We need another circle hidden behind to draw the outline on. We can't use
                        // the original circle because the shader draw over the outline.

    RenderingDef::Shader m_shader;
    sf::Clock m_shaderClock;

    DirectionArrows m_directionArrow;
    DirectionLines m_directionLines;

    float m_resourceCounts[RESOURCE_TYPE_COUNT] = {0}; // Used to pass resource counts to shader
};

#endif /* DrawableGroup_hpp */
