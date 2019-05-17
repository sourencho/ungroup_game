#include <stdio.h>
#include <iostream>
#include "GroupShape.hpp"

GroupShape::GroupShape(
    float size,
    sf::Vector2f position,
    sf::Color color
):mCircle(size) {
    mCircle.setPosition(position);
    mCircle.setFillColor(color);
    mIsActive = false;
}

GroupShape::~GroupShape() {
    //dtor
}

void GroupShape::draw(sf::RenderTarget& target) {
    target.draw(mCircle);
}

sf::Vector2f GroupShape::getPosition() const {
    return mCircle.getPosition();
}

float GroupShape::getRadius() const {
    return mCircle.getRadius();
}

void GroupShape::setPosition(sf::Vector2f position) {
    mCircle.setPosition(position);
}


void GroupShape::setRadius(int size) {
    mCircle.setRadius(size);
}

void GroupShape::setActive(bool is_active) {
    mIsActive = is_active;
}

bool GroupShape::isActive() const {
    return mIsActive;
}
