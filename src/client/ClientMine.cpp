#include <stdio.h>
#include <iostream>
#include "ClientMine.hpp"

ClientMine::ClientMine(sf::Vector2f position, sf::Color color)
    : CircleGameObject(0, position, 0.f, color) {}
ClientMine::~ClientMine() {}
