#include "MineController.hpp"

#include <exception>

#include "../factories/IdFactory.hpp"

MineController::MineController(std::vector<std::shared_ptr<Mine>> &mines) : mMines(mines) {}

MineController::~MineController() {}

uint32_t MineController::createMine() {
    size_t next_mine_index = nextMineIndex++;
    if (next_mine_index >= mMines.size()) {
        throw std::out_of_range("Exceeded max number of mines.");
    }

    uint32_t new_mine_id = mMines[next_mine_index]->getId();
    getMine(new_mine_id)->setActive(true);
    return new_mine_id;
}

void MineController::update() {
    // noop
}

void MineController::updatePostPhysics() {
    for (auto &mine : mMines) {
        mine->matchRigid();
    }
}

std::shared_ptr<Mine> &MineController::getMine(uint32_t mine_id) {
    return mMines[IdFactory::getInstance().getIndex(mine_id)];
}