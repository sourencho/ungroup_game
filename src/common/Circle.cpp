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

// TODO(souren): On client use rigid body instead of this
void Circle::move() {
    mCircleShape.setPosition(mCircleShape.getPosition() + mVelocity);
}

// TODO(souren): On client use rigid body instead of this
void Circle::move(sf::Vector2f offset) {
    mCircleShape.setPosition(mCircleShape.getPosition() + offset);
}

// TODO(souren): On client use rigid body instead of this
sf::Vector2f Circle::getVelocity() const {
    return mVelocity;
}

// TODO(souren): On client use rigid body instead of this
void Circle::setVelocity(sf::Vector2f velocity) {
    mVelocity = velocity;
}
