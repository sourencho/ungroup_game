#ifndef GameObjectStore_hpp
#define GameObjectStore_hpp

#include <memory>
#include <vector>

#include "../objects/Group.hpp"
#include "../objects/Mine.hpp"
#include "../objects/Player.hpp"
#include "../resources/ResourceStore.hpp"

class GameObjectStore {
  public:
    explicit GameObjectStore(std::shared_ptr<PhysicsController> pc, ResourceStore& rs);
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
    ResourceStore& mResourceStore;

    unsigned int mNextPlayerId = 0;
};

#endif /* GameObjectStore_hpp */
