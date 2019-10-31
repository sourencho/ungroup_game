#include <memory>
#include <vector>

#include "../../common/factories/IdFactory.hpp"
#include "../../common/util/game_def.hpp"
#include "../../common/util/game_settings.hpp"
#include "GameObjectStore.hpp"

GameObjectStore::GameObjectStore(std::shared_ptr<PhysicsController> pc, ResourceStore& rs)
    : mPhysicsController(pc), mResourceStore(rs) {}

GameObjectStore::~GameObjectStore() {}

void GameObjectStore::loadLevel(size_t max_player_count, size_t max_mine_count) {
    mPlayers.reserve(max_player_count);
    mGroups.reserve(max_player_count);
    mMines.reserve(max_mine_count);

    // Initialize Players
    for (int i = 0; i < max_player_count; i++) {
        uint32_t new_player_id = IdFactory::getInstance().getNextId(GameObjectType::player);
        mPlayers.push_back(std::shared_ptr<Player>(new Player(new_player_id)));
    }

    // Initialize Groups
    for (int i = 0; i < max_player_count; i++) {
        uint32_t new_group_id = IdFactory::getInstance().getNextId(GameObjectType::group);
        mGroups.push_back(std::shared_ptr<Group>(new Group(
            new_group_id, sf::Vector2f(GROUP_START_OFFSET_X * (i + 1), GROUP_START_OFFSET_Y),
            sf::Color(0, 255, 0), mPhysicsController, mResourceStore)));
    }

    // Initialize Mines
    for (int i = 0; i < max_mine_count; i++) {
        uint32_t new_mine_id = IdFactory::getInstance().getNextId(GameObjectType::mine);
        mMines.push_back(std::shared_ptr<Mine>(
            new Mine(new_mine_id, sf::Vector2f(MINE_START_OFFSET_X, MINE_START_OFFSET_Y * (i + 1)),
                     MINE_SIZE, sf::Color(0, 0, 255), mPhysicsController, mResourceStore)));
    }
}

std::shared_ptr<Player>& GameObjectStore::getPlayer(uint32_t player_id) {
    return mPlayers[IdFactory::getInstance().getIndex(player_id)];
}

std::shared_ptr<Group>& GameObjectStore::getGroup(uint32_t group_id) {
    return mGroups[IdFactory::getInstance().getIndex(group_id)];
}

std::shared_ptr<Mine>& GameObjectStore::getMine(uint32_t mine_id) {
    return mMines[IdFactory::getInstance().getIndex(mine_id)];
}

std::vector<std::shared_ptr<Player>>& GameObjectStore::getPlayers() { return mPlayers; }

std::vector<std::shared_ptr<Group>>& GameObjectStore::getGroups() { return mGroups; }

std::vector<std::shared_ptr<Mine>>& GameObjectStore::getMines() { return mMines; }
