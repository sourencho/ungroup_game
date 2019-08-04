#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include <numeric>

#include "Mine.hpp"
#include "../common/game_settings.hpp"

Mine::Mine(unsigned int id, sf::Vector2f position, float size, std::shared_ptr<CircleRigidBody> crb)
    :CircleGameObject(id, position, size, crb) {}

Mine::~Mine() {}

std::vector<std::shared_ptr<Mine>> Mine::getActiveMines(std::vector<std::shared_ptr<Mine>>& mines) {
    std::vector<std::shared_ptr<Mine>> active_mines;
    std::copy_if(
        mines.begin(), mines.end(), std::back_inserter(active_mines),
        [](std::shared_ptr<Mine> mine){return mine->isActive();});
    return active_mines;
}
