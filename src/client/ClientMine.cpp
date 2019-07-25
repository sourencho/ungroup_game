#include <stdio.h>
#include <iostream>
#include "ClientMine.hpp"

ClientMine::ClientMine(sf::Vector2f position) {
    mCircle = new Circle(0.f, position);
}

ClientMine::~ClientMine() {}

Circle* ClientMine::getCircle() {
    return mCircle;
}
