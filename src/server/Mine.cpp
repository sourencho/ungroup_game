#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include <numeric>
#include "Mine.hpp"
#include "../common/collision.hpp"
#include "../common/game_settings.hpp"

Mine::Mine(int id, sf::Vector2f position, float size)
    :mCircle(std::shared_ptr<Circle>(new Circle(size, position))) {
    mId = id;
}

Mine::~Mine() {}

int Mine::getId() const {
    return mId;
}

std::shared_ptr<Circle> Mine::getCircle() {
    return mCircle;
}

std::vector<std::shared_ptr<Mine>> Mine::getActiveMines(std::vector<std::shared_ptr<Mine>>& mines) {
    std::vector<std::shared_ptr<Mine>> active_mines;
    std::copy_if(
        mines.begin(), mines.end(), std::back_inserter(active_mines),
        [](std::shared_ptr<Mine> mine){return mine->isActive();});
    return active_mines;
}

std::vector<std::shared_ptr<Circle>> Mine::getCircles(std::vector<std::shared_ptr<Mine>>& mines) {
    std::vector<std::shared_ptr<Circle>> circles;
    std::transform(
        mines.begin(), mines.end(), std::back_inserter(circles),
        [](std::shared_ptr<Mine> mine){return mine->getCircle();});
    return circles;
}
