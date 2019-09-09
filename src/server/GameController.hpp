#ifndef GameController_hpp
#define GameController_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include <unordered_map>

#include "../common/Player.hpp"
#include "../common/Group.hpp"
#include "../common/Mine.hpp"
#include "NetworkingServer.hpp"
#include "LevelController.hpp"
#include "../common/PhysicsController.hpp"
#include "../common/events/Event.hpp"


class GameController {
 public:
     explicit GameController(size_t max_player_count, size_t max_mine_count);
     ~GameController();
     GameController(const GameController& temp_obj) = delete;
     GameController& operator=(const GameController& temp_obj) = delete;

     void step();
     void clientConnected(std::shared_ptr<Event> event);
     void clientDisconnected(std::shared_ptr<Event> event);

 private:
     ClientInputs collectInputs();
     void computeGameState(const ClientInputs& cis);
     void refreshAndUpdateGroups();
     void setNetworkState();
     void incrementTick();

     void updateGameObjects(const ClientInputs& cis);
     void updateGameObjectsPostPhysics();
     PlayerUpdate clientUpdateToPlayerUpdate(ClientUnreliableUpdate client_unreliable_update,
       ClientReliableUpdate client_reliable_update);

     void updatePlayers(const ClientInputs& cis);
     void updateGroups();

     unsigned int createPlayerWithGroup();

     std::unique_ptr<NetworkingServer> mNetworkingServer;
     std::shared_ptr<PhysicsController> mPhysicsController;
     std::unique_ptr<LevelController> mLevelController;

     std::unordered_map<sf::Uint32, int> mClientToPlayer;

};

#endif /* GameController_hpp */
