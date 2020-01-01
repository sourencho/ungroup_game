#include "LevelController.hpp"

#include "../physics/VectorUtil.hpp"
#include "../util/game_settings.hpp"

LevelController::LevelController(GroupController& gc, MineController& mc) :
    m_groupController(gc), m_mineController(mc) {
}

void LevelController::loadLevel(LevelKey level_key) {
    if (m_levelLoaders.count(level_key) == 0) {
        throw std::runtime_error("Level with used key doesn't exist");
    }
    (m_levelLoaders[level_key])();
}

void LevelController::loadLevelEmpty() {
}

void LevelController::loadLevelMineRing() {
    // Create mines in a ring around the center of the game
    for (int i = 0; i < MAX_MINE_COUNT; i++) {
        float radius = GAME_BOUNDS_RADIUS * 0.8f;
        float angle = i * 360.f / MAX_MINE_COUNT;
        sf::Vector2f direction = VectorUtil::direction(angle);
        sf::Vector2f mine_center_position = GAME_CENTER + direction * radius;
        m_mineController.createMine(mine_center_position);
    }
}