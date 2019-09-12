#include <stdio.h>
#include <random>
#include <iostream>
#include <utility>

#include "Circle.hpp"


Circle::Circle(float size, sf::Vector2f position, sf::Color color):
  mCircleShape(size) {
    mCircleShape.setPosition(position);
    mColor = color;
    mCircleShape.setFillColor(color);
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


sf::Vector2f Circle::getPosition() const {
    return mCircleShape.getPosition();
}

float Circle::getRadius() const {
    return mCircleShape.getRadius();
}

void Circle::setColor(sf::Color color) {
    mColor = color;
    mCircleShape.setFillColor(color);
}

void Circle::setColor() {
    mCircleShape.setFillColor(mColor);
}

void Circle::changeColor(sf::Color color) {
    mCircleShape.setFillColor(color);
}

void Circle::setPosition(sf::Vector2f position) {
    mCircleShape.setPosition(position);
}

void Circle::setRadius(int size) {
    mCircleShape.setRadius(size);
}
