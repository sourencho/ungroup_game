#include <memory>
#include <vector>

#include "LevelController.hpp"
#include "../common/game_settings.hpp"

LevelController::LevelController(std::shared_ptr<PhysicsController> pc):
  mPhysicsController(pc) {}

LevelController::~LevelController() {}

void LevelController::loadLevel(size_t max_player_count, size_t max_mine_count) {
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

int LevelController::createPlayer() {
    int new_player_id = mNextPlayerId++;
    if (new_player_id >= mPlayers.size() || new_player_id >= mGroups.size()) {
        throw std::runtime_error("Create players or groups with id out of range");
    }
    setPlayerActive(new_player_id, true);
    return new_player_id;
}

int LevelController::createGroup(int player_id) {
    int new_group_id = player_id;
    setGroupActive(new_group_id, true);
    mGroups[new_group_id]->addMember(mPlayers[player_id]);
    return new_group_id;
}

void LevelController::setPlayerActive(int player_id, bool value) {
    mPlayers[player_id]->setActive(value);
}


void LevelController::setGroupActive(int group_id, bool value) {
    mGroups[group_id]->setActive(value);
}

void LevelController::setMineActive(int mine_id, bool value) {
    mMines[mine_id]->setActive(value);
}

std::vector<int> LevelController::getPlayerIds() {
    std::vector<int> player_ids(mPlayers.size());
    std::transform(
        mPlayers.begin(), mPlayers.end(), std::back_inserter(player_ids),
        [](std::shared_ptr<Player> player){return player->getId();});
    return player_ids;
}

std::shared_ptr<Player> LevelController::getPlayer(int player_id) {
    return mPlayers[player_id];
}

std::vector<std::shared_ptr<Player>> LevelController::getPlayers() {
    return mPlayers;
}

std::vector<std::shared_ptr<Group>> LevelController::getGroups() {
    return mGroups;
}

std::vector<std::shared_ptr<Mine>> LevelController::getMines() {
    return mMines;
}

std::vector<std::shared_ptr<Player>> LevelController::getActivePlayers() {
    std::vector<std::shared_ptr<Player>> active_players;
    std::copy_if(
        mPlayers.begin(), mPlayers.end(), std::back_inserter(active_players),
        [](std::shared_ptr<Player> player){return player->isActive();});
    return active_players;
}
