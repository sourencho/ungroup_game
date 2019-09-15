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
#include "LevelController.hpp"


class GameController {
 public:
    explicit GameController(size_t max_player_count, size_t max_mine_count);
    ~GameController();
    GameController(const GameController& temp_obj) = delete;
    GameController& operator=(const GameController& temp_obj) = delete;

    void step();

 protected:
    virtual ClientInputs collectInputs() = 0;
    virtual void setNetworkState() = 0;
    virtual void incrementTick() = 0;

    void updatePlayers(const ClientInputs& cis);
    void updateGroups();

    void computeGameState(const ClientInputs& cis);
    unsigned int createPlayerWithGroup();
    void applyGameState(GameState game_state);
    void updateGameObjects(const ClientInputs& cis);
    void updateGameObjectsPostPhysics();
    PlayerUpdate clientUpdateToPlayerUpdate(ClientUnreliableUpdate client_unreliable_update,
      ClientReliableUpdate client_reliable_update);

    std::shared_ptr<PhysicsController> mPhysicsController;
    std::unique_ptr<LevelController> mLevelController;
    std::unordered_map<sf::Uint32, int> mClientToPlayer;
};

#endif /* GameController_hpp */
