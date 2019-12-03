#include "MineController.hpp"

#include <exception>

#include "../factories/IdFactory.hpp"
#include "../rendering/RenderingDef.hpp"
#include "../util/game_settings.hpp"

MineController::MineController(std::vector<std::shared_ptr<Mine>>& mines, ResourceController& rc) :
    m_mines(mines), m_resourceController(rc) {
    for (int i = 0; i < MAX_MINE_COUNT; i++) {
        createMine();
    }
}

MineController::~MineController() {
}

uint32_t MineController::createMine() {
    size_t next_mine_index = nextMineIndex++;
    if (next_mine_index >= m_mines.size()) {
        throw std::out_of_range("Exceeded max number of mines.");
    }

    Mine& new_mine = *m_mines[next_mine_index];
    uint32_t new_mine_id = new_mine.getId();
    new_mine.setActive(true);
    m_resourceController.add(new_mine_id, new_mine.getResourceType(), MINE_RESOURCE_COUNT);
    return new_mine_id;
}

void MineController::draw(sf::RenderTarget& target) {
    for (auto& mine : m_mines) {
        mine->draw(target);
    }
}

void MineController::update() {
    for (auto& mine : m_mines) {
        mine->setResourceCount(m_resourceController.get(mine->getId(), mine->getResourceType()));
    }
}

void MineController::updatePostPhysics() {
    for (auto& mine : m_mines) {
        mine->matchRigid();
    }
}

Mine& MineController::getMine(uint32_t mine_id) {
    return *m_mines[IdFactory::getInstance().getIndex(mine_id)];
}