#include <stdio.h>
#include <iostream>
#include "ClientGroup.hpp"

ClientGroup::ClientGroup(sf::Vector2f position, std::shared_ptr<PhysicsController> pc)
    :CircleGameObject(0, position, 0.f, pc) {}

ClientGroup::~ClientGroup() {}
