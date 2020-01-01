/**
 * Loads levels for game. A level being an initial layout of game objects.
 * To define a new level, create a LevelKey a LoadLevelX method and add them to the level loaders
 * map.
 */

#ifndef LevelController_hpp
#define LevelController_hpp

#include <functional>

#include "GroupController.hpp"
#include "MineController.hpp"

enum LevelKey { mine_ring, empty };

class LevelController {
  public:
    explicit LevelController(GroupController& gc, MineController& mc);
    ~LevelController(){};
    LevelController(const LevelController& temp_obj) = delete;
    LevelController& operator=(const LevelController& temp_obj) = delete;

    /**
     * Runs the loadLevel method corresponding to to the level_key
     */
    void loadLevel(LevelKey level_key);

  private:
    /* Level loading methods */
    void loadLevelMineRing();
    void loadLevelEmpty();

    std::map<LevelKey, std::function<void()>> m_levelLoaders = {
        {LevelKey::mine_ring, std::bind(&LevelController::loadLevelMineRing, this)},
        {LevelKey::empty, std::bind(&LevelController::loadLevelEmpty, this)}};

    GroupController& m_groupController;
    MineController& m_mineController;
};

#endif /* LevelController_hpp */