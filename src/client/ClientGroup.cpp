#include <stdio.h>
#include <iostream>
#include "ClientGroup.hpp"

ClientGroup::ClientGroup(sf::Vector2f position):CircleGameObject(0, position, 0.f) {}

ClientGroup::~ClientGroup() {}
