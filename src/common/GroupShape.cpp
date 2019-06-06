#include <stdio.h>
#include <iostream>
#include "GroupShape.hpp"

GroupShape::GroupShape(
    float size,
    sf::Vector2f position
):mCircle(size) {
    mCircle.setPosition(position);
    mCircle.setFillColor(
        sf::Color(rand() % 255, rand() % 255, rand() % 255)
    );
    mVelocity = sf::Vector2f(0.f, 0.f);
}

GroupShape::~GroupShape() {
    //dtor
}

void GroupShape::draw(sf::RenderTarget& target) {
    target.draw(mCircle);
}

sf::Vector2f GroupShape::getVelocity() const {
    return mVelocity;
}

sf::Vector2f GroupShape::getPosition() const {
    return mCircle.getPosition();
}

float GroupShape::getRadius() const {
    return mCircle.getRadius();
}

void GroupShape::setVelocity(sf::Vector2f velocity) {
    mVelocity = velocity;
}

void GroupShape::setPosition(sf::Vector2f position) {
    mCircle.setPosition(position);
}

void GroupShape::setRadius(int size) {
    mCircle.setRadius(size);
}

void GroupShape::move() {
    mCircle.setPosition(mCircle.getPosition() + mVelocity);
}

void GroupShape::move(sf::Vector2f offset) {
    mCircle.setPosition(mCircle.getPosition() + offset);
}
