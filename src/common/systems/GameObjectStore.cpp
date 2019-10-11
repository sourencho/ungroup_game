#include <memory>
#include <vector>

#include "GameObjectStore.hpp"
#include "../../common/util/game_settings.hpp"

GameObjectStore::GameObjectStore(std::shared_ptr<PhysicsController> pc):
  mPhysicsController(pc) {}

GameObjectStore::~GameObjectStore() {}

void GameObjectStore::loadLevel(size_t max_player_count, size_t max_mine_count) {
    mPlayers.reserve(max_player_count);
    mGroups.reserve(max_player_count);
    mMines.reserve(max_mine_count);

    // Initialize Players
    for (int i=0; i < max_player_count; i++) {
        mPlayers.push_back(std::shared_ptr<Player>(new Player(i)));
    }

    // Initialize Groups
    for (int i=0; i < max_player_count; i++) {
        mGroups.push_back(std::shared_ptr<Group>(new Group(
            i,
            sf::Vector2f(GROUP_START_OFFSET_X * (i+1), GROUP_START_OFFSET_Y),
            sf::Color(0, 255, 0),
            mPhysicsController)));
    }

    // Initialize Mines
    for (int i=0; i < max_mine_count; i++) {
        std::shared_ptr<Mine> new_mine = std::shared_ptr<Mine>(new Mine(
            i,
            sf::Vector2f(MINE_START_OFFSET_X, MINE_START_OFFSET_Y * (i+1)),
            MINE_SIZE,
            sf::Color(0, 0, 255),
            mPhysicsController));
        mMines.push_back(new_mine);
        setMineActive(i, true);
    }
}

int GameObjectStore::createPlayer() {
    int new_player_id = mNextPlayerId++;
    if (new_player_id >= mPlayers.size() || new_player_id >= mGroups.size()) {
        throw std::runtime_error("Create players or groups with id out of range");
    }
    setPlayerActive(new_player_id, true);
    return new_player_id;
}

void GameObjectStore::setPlayerActive(int player_id, bool value) {
    mPlayers[player_id]->setActive(value);
}


void GameObjectStore::setMineActive(int mine_id, bool value) {
    mMines[mine_id]->setActive(value);
}

std::shared_ptr<Player>& GameObjectStore::getPlayer(int player_id) {
    return mPlayers[player_id];
}

std::shared_ptr<Group>& GameObjectStore::getGroup(int group_id) {
    return mGroups[group_id];
}

std::shared_ptr<Mine>& GameObjectStore::getMine(int mine_id) {
    return mMines[mine_id];
}

std::vector<std::shared_ptr<Player>>& GameObjectStore::getPlayers() {
    return mPlayers;
}

std::vector<std::shared_ptr<Group>>& GameObjectStore::getGroups() {
    return mGroups;
}

std::vector<std::shared_ptr<Mine>>& GameObjectStore::getMines() {
    return mMines;
}
