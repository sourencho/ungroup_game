#include "MineController.hpp"

#include <exception>
#include <iostream>

#include "../factories/IdFactory.hpp"
#include "../physics/VectorUtil.hpp"
#include "../util/game_settings.hpp"

MineController::MineController(std::vector<std::shared_ptr<Mine>>& mines, ResourceController& rc) :
    m_mines(mines), m_resourceController(rc) {
}

MineController::~MineController() {
}

uint32_t MineController::createMine(sf::Vector2f center_position) {
    size_t next_mine_index = nextMineIndex++;
    if (next_mine_index >= m_mines.size()) {
        throw std::out_of_range("Exceeded max number of mines.");
    }

    Mine& new_mine = *m_mines[next_mine_index];
    uint32_t new_mine_id = new_mine.getId();
    new_mine.setCenterPosition(center_position);
    new_mine.setActive(true);
    m_resourceController.add(new_mine_id, new_mine.getResourceType(),
                             new_mine.getResourceCapacity());
    return new_mine_id;
}

void MineController::update() {
    for (auto& mine : m_mines) {
        mine->setResourceCount(m_resourceController.get(mine->getId(), mine->getResourceType()));
    }
}

Mine& MineController::getMine(uint32_t mine_id) {
    return *m_mines[IdFactory::getInstance().getIndex(mine_id)];
}

std::vector<uint32_t> MineController::getMineIds() {
    std::vector<uint32_t> mine_ids;
    std::transform(m_mines.begin(), m_mines.end(), std::back_inserter(mine_ids),
                   [this](std::shared_ptr<Mine> mine) -> uint32_t { return mine->getId(); });
    return mine_ids;
}