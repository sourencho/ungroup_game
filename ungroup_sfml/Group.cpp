#include <stdio.h>
#include "Group.h"

Group::Group(float size, sf::Color color) : mCircle(size) {
    mCircle.setFillColor(color);
}

Group::~Group() {
    //dtor
}

void Group::update() {
    mCircle.setPosition(mPosition);
}

void Group::draw(sf::RenderTarget& target) {
    target.draw(mCircle);
}

void Group::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::W) {
            mPosition.y -= 5.f;
        } else if (event.key.code == sf::Keyboard::S) {
            mPosition.y += 5.f;
        }
        
        if (event.key.code == sf::Keyboard::A) {
            mPosition.x -= 5.f;
        } else if (event.key.code == sf::Keyboard::D) {
            mPosition.x += 5.f;
        }
    }
}
