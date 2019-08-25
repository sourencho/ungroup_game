#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include <numeric>

#include "Mine.hpp"
#include "../common/game_settings.hpp"

Mine::Mine(unsigned int id, sf::Vector2f position, float size, sf::Color color,
  std::shared_ptr<PhysicsController> pc):
  CircleGameObject(id, position, size, color, pc) {
}

Mine::~Mine() {}
