#include "DirectionArrow.hpp"

#include <iostream>
#include <math.h>

#include "../physics/VectorUtil.hpp"

#define PI 3.14159265

const float MIN_SPEED = 1.f;            // Minimum speed at which arrow will still show
const float TWO_MINUS_SQRT_3 = 0.2679f; // 2 - sqrt(3);

DirectionArrow::DirectionArrow() : m_triangle(0, 3) {
}

void DirectionArrow::draw(sf::RenderTarget& render_target, float size, float radius,
                          sf::Vector2f position, sf::Vector2f velocity, sf::Color color,
                          bool active) {

    if (!active || (abs(velocity.x) < MIN_SPEED && abs(velocity.y) < MIN_SPEED)) {
        return;
    }

    sf::Vector2f direction = VectorUtil::normalize(velocity);

    m_triangle.setFillColor(color);
    m_triangle.setRadius(size);
    m_triangle.setOrigin(sf::Vector2f(size, size));

    float angle = atan2(direction.y, direction.x) * 180.f / PI;
    m_triangle.setRotation(angle - 270.f);

    m_triangle.setPosition(position.x + radius, position.y + radius);

    m_triangle.move(direction * radius + direction * (TWO_MINUS_SQRT_3 * size * 2.f));

    render_target.draw(m_triangle);
}