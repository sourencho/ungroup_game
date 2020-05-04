#include "DrawableMine.hpp"

#include "../../common/util/game_settings.hpp"
#include <numeric>

#include "RenderingDef.hpp"

DrawableMine::DrawableMine(ResourceStore& rs) : DrawableCircle(rs) {
    setTexture(RenderingDef::TextureKey::mine_pattern);
    setShader(RenderingDef::ShaderKey::voronoi_counts);
}

void DrawableMine::draw(sf::RenderTarget& target, Mine& mine, uint32_t resource_count,
                        const std::array<uint32_t, RESOURCE_TYPE_COUNT>& resource_counts) {
    if (!mine.isActive()) {
        return;
    }

    // Circle
    m_circleShape.setPosition(mine.getPosition());
    m_circleShape.setRadius(mine.getRadius());

    if (m_shader.shader != nullptr && RenderingDef::USE_SHADERS) {
        std::copy(resource_counts.begin(), resource_counts.end(), m_resourceCounts);
        int total_resource_count = mine.getResourceCapacity();
        m_shader.shader->setUniform("u_resolution", sf::Vector2f(GAME_SETTINGS.GAME_SIZE));
        m_shader.shader->setUniform("u_position", mine.getPosition());
        m_shader.shader->setUniform("u_radius", mine.getRadius());
        m_shader.shader->setUniform("u_time", m_shaderClock.getElapsedTime().asSeconds());
        m_shader.shader->setUniformArray("u_resourceCounts", m_resourceCounts, RESOURCE_TYPE_COUNT);
        m_shader.shader->setUniform("u_maxResources", total_resource_count);
    }

    if (m_shader.shader != nullptr && RenderingDef::USE_SHADERS) {
        target.draw(m_circleShape, m_shader.shader.get());
    } else {
        target.draw(m_circleShape);
    }

    // Outline
    m_outlineShape.setPosition(mine.getPosition());
    m_outlineShape.setRadius(mine.getRadius());
    m_outlineShape.setOutlineThickness(RenderingDef::MINE_OUTLINE_THICKNESS);
    m_outlineShape.setOutlineColor(RenderingDef::RESOURCE_COLORS[mine.getResourceType()]);

    target.draw(m_outlineShape);
}