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
     void computeGameState(
         const std::vector<int>& client_ids,
         const std::vector<client_direction_update>& client_direction_updates);
     void setNetworkState();
     void incrementTick();

     void updateGameObjects(
         const std::vector<int>& client_ids,
         const std::vector<client_direction_update>& client_direction_updates);
     void updateGameObjectsPostPhysics();

     void refreshPlayers(std::vector<int> client_ids);
     void updatePlayers(std::vector<client_direction_update> client_direction_updates);
     void updateGroups();

     unsigned int assignPlayer();

     std::unique_ptr<NetworkingServer> mNetworkingServer;
     std::shared_ptr<PhysicsController> mPhysicsController;

     std::unordered_map<sf::Uint32, int> mClientToPlayer;

     std::vector<std::shared_ptr<Player>> mPlayers;
     std::vector<std::shared_ptr<Group>> mGroups;
     std::vector<std::shared_ptr<Mine>> mMines;

     unsigned int mNextPlayerId = 0;
};

#endif /* GameController_hpp */
