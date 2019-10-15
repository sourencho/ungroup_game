#ifndef GameObjectStore_hpp
#define GameObjectStore_hpp

#include <vector>
#include <memory>

#include "../../common/objects/Player.hpp"
#include "../../common/objects/Mine.hpp"
#include "../../common/objects/Group.hpp"


class GameObjectStore {
 public:
    explicit GameObjectStore(std::shared_ptr<PhysicsController> pc);
    ~GameObjectStore();
    GameObjectStore(const GameObjectStore& temp_obj) = delete;
    GameObjectStore& operator=(const GameObjectStore& temp_obj) = delete;

    void loadLevel(size_t max_player_count, size_t max_mine_count);

    std::shared_ptr<Player>& getPlayer(uint32_t player_id);
    std::shared_ptr<Group>& getGroup(uint32_t group_id);
    std::shared_ptr<Mine>& getMine(uint32_t mine_id);

    std::vector<std::shared_ptr<Player>>& getPlayers();
    std::vector<std::shared_ptr<Group>>& getGroups();
    std::vector<std::shared_ptr<Mine>>& getMines();

 private:
    std::vector<std::shared_ptr<Player>> mPlayers;
    std::vector<std::shared_ptr<Group>> mGroups;
    std::vector<std::shared_ptr<Mine>> mMines;

    std::shared_ptr<PhysicsController> mPhysicsController;

    unsigned int mNextPlayerId = 0;
};

#endif /* GameObjectStore_hpp */
