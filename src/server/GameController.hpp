#ifndef GameController_hpp
#define GameController_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include <unordered_map>

#include "Player.hpp"
#include "Group.hpp"
#include "NetworkingServer.hpp"


class GameController {
 public:
     explicit GameController(int max_player_count);
     ~GameController();
     GameController(const GameController& temp_obj) = delete;
     GameController& operator=(const GameController& temp_obj) = delete;
     void update();

 private:
     int createPlayer();
     client_inputs collectInputs();
     void computeGameState(
         const std::vector<int>& client_ids,
         const std::vector<client_direction_update>& client_direction_updates);
     void refreshPlayers(std::vector<int> client_ids);
     void updatePlayers(std::vector<client_direction_update> client_direction_updates);
     void refreshAndUpdateGroups();
     void setNetworkState();
     void incrementTick();
     std::vector<std::shared_ptr<Group>> getActiveGroups();

     std::vector<std::shared_ptr<Player>> mPlayers;
     std::vector<std::shared_ptr<Group>> mGroups;
     std::unique_ptr<NetworkingServer> mNetworkingServer;
     std::unordered_map<sf::Uint32, int> mClientToPlayer;
     size_t mNextPlayerId = 0;
};

#endif /* GameController_hpp */
