#include "MineController.hpp"

#include <exception>

#include "../factories/IdFactory.hpp"
#include "../rendering/RenderingDef.hpp"

MineController::MineController(std::vector<std::shared_ptr<Mine>>& mines) : m_Mines(mines) {}

MineController::~MineController() {}

uint32_t MineController::createMine() {
    size_t next_mine_index = nextMineIndex++;
    if (next_mine_index >= m_Mines.size()) {
        throw std::out_of_range("Exceeded max number of mines.");
    }

    Mine& new_mine = *m_Mines[next_mine_index];
    uint32_t new_mine_id = new_mine.getId();
    new_mine.setActive(true);
    return new_mine_id;
}

void MineController::draw(sf::RenderTarget& target) {
    for (auto& mine : m_Mines) {
        mine->draw(target);
    }
}

void MineController::update() {
    // noop
}

void MineController::updatePostPhysics() {
    for (auto& mine : m_Mines) {
        mine->matchRigid();
    }
}

std::shared_ptr<Mine>& MineController::getMine(uint32_t mine_id) {
    return m_Mines[IdFactory::getInstance().getIndex(mine_id)];
}