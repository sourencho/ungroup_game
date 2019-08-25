#ifndef GameController_hpp
#define GameController_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include <unordered_map>

#include "Player.hpp"
#include "Group.hpp"
#include "Mine.hpp"
#include "NetworkingServer.hpp"
#include "LevelController.hpp"
#include "../common/PhysicsController.hpp"


class GameController {
 public:
     explicit GameController(size_t max_player_count, size_t max_mine_count);
     ~GameController();
     GameController(const GameController& temp_obj) = delete;
     GameController& operator=(const GameController& temp_obj) = delete;

     void step();

 private:
     void loadLevel(size_t max_player_count, size_t max_mine_count);

     client_inputs collectInputs();
     void computeGameState(const client_inputs& cis);
     void updatePlayers(const client_inputs& cis);
     void refreshAndUpdateGroups();
     void setNetworkState();
     void incrementTick();

     void updateGameObjects(const client_inputs& cis);
     void updateGameObjectsPostPhysics();

     void updateGroups();

     unsigned int assignPlayer();

     std::unique_ptr<NetworkingServer> mNetworkingServer;
     std::shared_ptr<PhysicsController> mPhysicsController;
     std::unique_ptr<LevelController> mLevelController;

     std::unordered_map<sf::Uint32, int> mClientToPlayer;

};

#endif /* GameController_hpp */
