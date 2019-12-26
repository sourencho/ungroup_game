#include "DrawableCircle.hpp"

DrawableCircle::DrawableCircle(ResourceStore& rs) : m_resourceStore(rs) {
    m_shaderClock.restart();
    m_outlineShape.setFillColor(sf::Color::Transparent);
    m_circleShape.setFillColor(sf::Color::Transparent);
}

void DrawableCircle::setShader(RenderingDef::ShaderKey shader_key) {
    m_shader.key = shader_key;
    m_shader.shader = m_resourceStore.getShader(shader_key);
}

void DrawableCircle::setTexture(RenderingDef::TextureKey texture_key) {
    m_circleShape.setTexture(m_resourceStore.getTexture(texture_key).get());
}