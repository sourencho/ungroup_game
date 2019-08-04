#include <stdio.h>
#include <iostream>
#include "ClientGroup.hpp"

ClientGroup::ClientGroup(sf::Vector2f position, std::shared_ptr<CircleRigidBody> crb)
    :CircleGameObject(0, position, 0.f, crb) {}

ClientGroup::~ClientGroup() {}
