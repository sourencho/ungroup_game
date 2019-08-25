#ifndef LevelController_hpp
#define LevelController_hpp

#include "Player.hpp"
#include "Group.hpp"
#include "Mine.hpp"

class LevelController {
 public:
     explicit LevelController(std::shared_ptr<PhysicsController> pc);
     ~LevelController();
     LevelController(const LevelController& temp_obj) = delete;
     LevelController& operator=(const LevelController& temp_obj) = delete;

     void loadLevel(size_t max_player_count, size_t max_mine_count);

     int createPlayer();
     int createGroup(int player_id);

     void setPlayerActive(int player_id, bool value);
     void setGroupActive(int group_id, bool value);
     void setMineActive(int mine_id, bool value);

     std::vector<int> getPlayerIds();

     std::shared_ptr<Player> getPlayer(int player_id);

     std::vector<std::shared_ptr<Player>> getPlayers();
     std::vector<std::shared_ptr<Group>> getGroups();
     std::vector<std::shared_ptr<Mine>> getMines();

     std::vector<std::shared_ptr<Player>> getActivePlayers();
     std::vector<std::shared_ptr<Group>> getActiveGroups();
     std::vector<std::shared_ptr<Mine>> getActiveMines();

 private:
     std::vector<std::shared_ptr<Player>> mPlayers;
     std::vector<std::shared_ptr<Group>> mGroups;
     std::vector<std::shared_ptr<Mine>> mMines;

     std::shared_ptr<PhysicsController> mPhysicsController;

     unsigned int mNextPlayerId = 0;
};

#endif /* LevelController_hpp */
