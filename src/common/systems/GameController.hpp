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
#include "GameObjectStore.hpp"
#include "GroupController.hpp"
#include "MineController.hpp"
#include "PlayerController.hpp"

class GameController {
  public:
    // Game state is stepped at least every <MIN_TIME_STEP> milliseconds.
    sf::Int32 MIN_TIME_STEP = (sf::Int32)((1 / 60.) * 1000.);

    explicit GameController(size_t max_player_count, size_t max_mine_count);
    ~GameController();
    GameController(const GameController& temp_obj) = delete;
    GameController& operator=(const GameController& temp_obj) = delete;

    virtual void update();

  protected:
    virtual PlayerInputs collectInputs() = 0;
    virtual void setNetworkState() = 0;
    virtual void incrementTick() = 0;
    virtual unsigned int getTick() = 0;
    virtual void setTick(unsigned int tick) = 0;
    virtual void step(const PlayerInputs& pi, sf::Int32 delta_ms) = 0;

    void updatePlayers(const PlayerInputs& pi);
    void updateGroups();
    void computeGameState(const PlayerInputs& pi, sf::Int32 delta_ms);
    uint32_t createPlayerWithGroup(uint32_t client_id);
    void applyGameState(GameState game_state);
    GameState getGameState();
    void updateGameObjects(const PlayerInputs& pi);
    void updateGameObjectsPostPhysics();
    PlayerUpdate clientUpdateToPlayerUpdate(ClientUnreliableUpdate client_unreliable_update,
                                            ClientReliableUpdate client_reliable_update);
    void collisionEvent(std::shared_ptr<Event> event);
    void applyCollisionForce(uint32_t collider_a_id, uint32_t collider_b_id);

    std::shared_ptr<PhysicsController> m_physicsController;
    std::unique_ptr<GameObjectStore> m_gameObjectStore;
    std::unique_ptr<GroupController> m_groupController;
    std::unique_ptr<PlayerController> m_playerController;
    std::unique_ptr<MineController> m_mineController;
    std::unique_ptr<ResourceStore> m_resourceStore;

    sf::Clock m_clock;
    sf::Int32 m_elapsedTime = 0;
    sf::Int32 m_timeAccumulator = 0;
};

#endif /* GameController_hpp */
