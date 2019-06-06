#include <stdio.h>
#include <iostream>
#include "Circle.hpp"

Circle::Circle(
    float size,
    sf::Vector2f position
):mCircleShape(size) {
    mCircleShape.setPosition(position);
    mCircleShape.setFillColor(
        sf::Color(rand() % 255, rand() % 255, rand() % 255)
    );
    mVelocity = sf::Vector2f(0.f, 0.f);
}

Circle::~Circle() {
    //dtor
}

void Circle::draw(sf::RenderTarget& target) {
    target.draw(mCircleShape);
}

sf::Vector2f Circle::getVelocity() const {
    return mVelocity;
}

sf::Vector2f Circle::getPosition() const {
    return mCircleShape.getPosition();
}

float Circle::getRadius() const {
    return mCircleShape.getRadius();
}

void Circle::setVelocity(sf::Vector2f velocity) {
    mVelocity = velocity;
}

void Circle::setPosition(sf::Vector2f position) {
    mCircleShape.setPosition(position);
}

void Circle::setRadius(int size) {
    mCircleShape.setRadius(size);
}

void Circle::move() {
    mCircleShape.setPosition(mCircleShape.getPosition() + mVelocity);
}

void Circle::move(sf::Vector2f offset) {
    mCircleShape.setPosition(mCircleShape.getPosition() + offset);
}
