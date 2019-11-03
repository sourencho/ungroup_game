#include "Circle.hpp"

#include <iostream>
#include <random>
#include <stdio.h>
#include <utility>

#include "../rendering/RenderingDef.hpp"
#include "../util/game_settings.hpp"

sf::Clock shader_clock;

Circle::Circle(float size, sf::Vector2f position, sf::Color color)
    : mCircleShape(size, RenderingDef::CIRCLE_POINT_COUNT) {
    mCircleShape.setPosition(position);
    mColor = color;
    mCircleShape.setFillColor(color);

    shader_clock.restart();
}

Circle::~Circle() {}

void Circle::draw(sf::RenderTarget& target) { target.draw(mCircleShape); }

void Circle::draw(sf::RenderTarget& target, sf::Shader& shader) {
    if (RenderingDef::USE_SHADERS) {
        shader.setUniform("u_resolution", sf::Vector2f(WINDOW_RESOLUTION));
        shader.setUniform("u_position", getPosition());
        shader.setUniform("u_radius", getRadius());
        shader.setUniform("u_time", shader_clock.getElapsedTime().asSeconds());
        target.draw(mCircleShape, &shader);
    } else {
        draw(target);
    }
}

sf::Vector2f Circle::getPosition() const { return mCircleShape.getPosition(); }

float Circle::getRadius() const { return mCircleShape.getRadius(); }

void Circle::setColor(sf::Color color) {
    mColor = color;
    mCircleShape.setFillColor(color);
}

void Circle::setColor() { mCircleShape.setFillColor(mColor); }

void Circle::changeColor(sf::Color color) { mCircleShape.setFillColor(color); }

void Circle::setPosition(sf::Vector2f position) { mCircleShape.setPosition(position); }

void Circle::setRadius(int size) { mCircleShape.setRadius(size); }

sf::Vector2f Circle::getCenter() const {
    sf::Vector2f position = mCircleShape.getPosition();
    float radius = mCircleShape.getRadius();
    return sf::Vector2f(position.x + radius, position.y + radius);
}

void Circle::setTexture(std::shared_ptr<sf::Texture> texture) {
    mCircleShape.setTexture(texture.get(), false);
}