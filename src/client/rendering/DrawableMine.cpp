#include "DrawableMine.hpp"

#include "RenderingDef.hpp"

DrawableMine::DrawableMine(ResourceStore& rs) : DrawableCircle(rs) {
    setTexture(RenderingDef::TextureKey::mine_pattern);
}

void DrawableMine::draw(sf::RenderTarget& target, Mine& mine, uint32_t resource_count) {
    if (!mine.isActive()) {
        return;
    }

    m_circleShape.setPosition(mine.getPosition());
    m_circleShape.setRadius(mine.getRadius());

    if (resource_count == 0) {
        m_circleShape.setFillColor(RenderingDef::EMPTY_MINE_COLOR);
    } else {
        m_circleShape.setFillColor(RenderingDef::RESOURCE_COLORS[mine.getResourceType()]);
    }

    target.draw(m_circleShape);
}