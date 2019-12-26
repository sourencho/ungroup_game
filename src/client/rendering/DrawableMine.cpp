#include "DrawableMine.hpp"

DrawableMine::DrawableMine(ResourceStore& rs) : m_resourceStore(rs) {
}

void DrawableMine::update(Mine& mine, uint32_t resource_count) {
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
}

void DrawableMine::draw(sf::RenderTarget& target, bool active) {
    if (active) {
        target.draw(m_circleShape);
    }
}
