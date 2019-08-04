#include <stdio.h>
#include <random>
#include <iostream>
#include <utility>

#include "Circle.hpp"

Circle::Circle(float size, sf::Vector2f position):mCircleShape(size) {
    mCircleShape.setPosition(position);
    unsigned int seed = time(NULL);
    mCircleShape.setFillColor(
        sf::Color(rand_r(&seed) % 255, rand_r(&seed) % 255, rand_r(&seed) % 255));
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

void Circle::setPosition(sf::Vector2f position) {
    mCircleShape.setPosition(position);
}

void Circle::setRadius(int size) {
    mCircleShape.setRadius(size);
}
