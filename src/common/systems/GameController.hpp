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
#include "../util/game_state.hpp"
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
    virtual ClientInputs collectInputs() = 0;
    virtual void setNetworkState() = 0;
    virtual void incrementTick() = 0;
    virtual unsigned int getTick() = 0;
    virtual void setTick(unsigned int tick) = 0;
    virtual void step(const ClientInputs& cis, sf::Int32 delta_ms) = 0;

    void updatePlayers(const ClientInputs& cis);
    void updateGroups();
    void computeGameState(const ClientInputs& cis, sf::Int32 delta_ms);
    uint32_t createPlayerWithGroup(uint32_t client_id);
    void applyGameState(GameState game_state);
    GameState getGameState();
    void updateGameObjects(const ClientInputs& cis);
    void updateGameObjectsPostPhysics();
    PlayerUpdate clientUpdateToPlayerUpdate(ClientUnreliableUpdate client_unreliable_update,
                                            ClientReliableUpdate client_reliable_update);
    void collisionEvent(std::shared_ptr<Event> event);
    void applyCollisionForce(uint32_t collider_a_id, uint32_t collider_b_id);

    std::shared_ptr<PhysicsController> mPhysicsController;
    std::unique_ptr<GameObjectStore> mGameObjectStore;
    std::unique_ptr<GroupController> mGroupController;
    std::unique_ptr<PlayerController> mPlayerController;
    std::unique_ptr<MineController> mMineController;
    std::unique_ptr<ResourceStore> mResourceStore;

    sf::Clock mClock;
    sf::Int32 mElapsedTime = 0;
    sf::Int32 mTimeAccumulator = 0;
};

#endif /* GameController_hpp */
