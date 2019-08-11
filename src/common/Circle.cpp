#include <stdio.h>
#include <random>
#include <iostream>
#include <utility>

#include "Circle.hpp"

Circle::Circle(float size, sf::Vector2f position):mCircleShape(size) {
    mCircleShape.setPosition(position);
    mColor = sf::Color(0, 0, 255);
    mCircleShape.setFillColor(mColor);
    mVelocity = sf::Vector2f(0.f, 0.f);
}

Circle::~Circle() {}

void Circle::draw(sf::RenderTarget& target, sf::Shader* shader, bool use_shader) {
    if (use_shader) {
        shader->setUniform("u_position", getPosition());
        shader->setUniform("u_radius", getRadius());
        target.draw(mCircleShape, shader);
    } else {
        target.draw(mCircleShape);
    }
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

void Circle::setColor(sf::Color color) {
    mColor = color;
    mCircleShape.setFillColor(mColor);
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
