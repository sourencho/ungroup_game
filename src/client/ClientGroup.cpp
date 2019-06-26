#include <stdio.h>
#include <iostream>
#include "ClientGroup.hpp"

ClientGroup::ClientGroup(sf::Vector2f position) {
    mCircle = new Circle(0.f, position);
}

ClientGroup::~ClientGroup() {}

Circle* ClientGroup::getCircle() {
    return mCircle;
}
