#ifndef GameController_hpp
#define GameController_hpp

#include <memory>
#include <stdio.h>
#include <unordered_map>
#include <vector>

#include "../events/Event.hpp"
#include "../objects/Group.hpp"
#include "../objects/Mine.hpp"
#include "../objects/Player.hpp"
#include "../physics/PhysicsController.hpp"
#include "../util/StateDef.hpp"
#include "GameObjectController.hpp"
#include "GameObjectStore.hpp"

class GameController {
  public:
    // Game state is stepped at least every <MIN_TIME_STEP> milliseconds.
    sf::Int32 MIN_TIME_STEP = (sf::Int32)((1 / 60.) * 1000.);

    explicit GameController();
    ~GameController();
    GameController(const GameController& temp_obj) = delete;
    GameController& operator=(const GameController& temp_obj) = delete;

    void step();

  protected:
    void computeGameState(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms);

    virtual void update(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) = 0;
    virtual void preUpdate() = 0;
    virtual void postUpdate() = 0;

    virtual InputDef::PlayerInputs getPlayerInputs() = 0;
    virtual void incrementTick() = 0;
    virtual unsigned int getTick() = 0;
    virtual void setTick(unsigned int tick) = 0;

    std::unique_ptr<PhysicsController> m_physicsController;
    std::unique_ptr<ResourceStore> m_resourceStore;
    std::unique_ptr<GameObjectController> m_gameObjectController;

    sf::Clock m_clock;
    sf::Int32 m_elapsedTime = 0;
    sf::Int32 m_timeAccumulator = 0;
};

#endif /* GameController_hpp */
