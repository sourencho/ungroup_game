#ifndef GameController_hpp
#define GameController_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include <unordered_map>

#include "../objects/Player.hpp"
#include "../objects/Group.hpp"
#include "../objects/Mine.hpp"
#include "../systems/PhysicsController.hpp"
#include "../util/game_state.hpp"
#include "GameObjectStore.hpp"
#include "GroupController.hpp"


class GameController {
 public:
    // Game state is stepped at least every <MIN_TIME_STEP> milliseconds.
    sf::Int32 MIN_TIME_STEP = (sf::Int32)((1/60.)*1000.);

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

    void updatePlayers(const ClientInputs& cis);
    void updateGroups();
    void computeGameState(const ClientInputs& cis, sf::Int32 delta_ms);
    unsigned int createPlayerWithGroup();
    void applyGameState(GameState game_state);
    GameState getGameState();
    void updateGameObjects(const ClientInputs& cis);
    void updateGameObjectsPostPhysics();
    PlayerUpdate clientUpdateToPlayerUpdate(ClientUnreliableUpdate client_unreliable_update,
      ClientReliableUpdate client_reliable_update);

    std::shared_ptr<PhysicsController> mPhysicsController;
    std::unique_ptr<GameObjectStore> mGameObjectStore;
    std::unique_ptr<GroupController> mGroupController;
    std::unordered_map<sf::Uint32, int> mClientToPlayer;

    sf::Clock mClock;
    sf::Int32 mElapsedTime = 0;
    sf::Int32 mTimeAccumulator = 0;
    unsigned int mStepCount = 0;
};

#endif /* GameController_hpp */
