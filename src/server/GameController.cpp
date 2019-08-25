#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <memory>

#include "GameController.hpp"
#include "../common/game_settings.hpp"


GameController::GameController(size_t max_player_count, size_t max_mine_count):
  mNetworkingServer(new NetworkingServer()),
  mPhysicsController(new PhysicsController()),
  mLevelController(new LevelController(mPhysicsController)) {
    mLevelController->loadLevel(max_player_count, max_mine_count);
}

GameController::~GameController() {}


void GameController::step() {
    client_inputs cis = collectInputs();
    computeGameState(cis.client_ids, cis.client_direction_updates, cis.client_groupability_updates);
    setNetworkState();
    incrementTick();
}

void GameController::computeGameState(
    const std::vector<int>& client_ids,
    const std::vector<client_direction_update>& client_direction_updates,
    const std::vector<client_groupability_update>& client_groupability_updates
) {
    updateGameObjects(client_ids, client_direction_updates, client_groupability_updates);
    mPhysicsController->step();
    mPhysicsController->handleCollision();
    updateGameObjectsPostPhysics();
}

client_inputs GameController::collectInputs() {
    return mNetworkingServer->collectClientInputs();
}

void GameController::incrementTick() {
    mNetworkingServer->incrementTick();
}

void GameController::updateGameObjects(const std::vector<int>& client_ids,
    const std::vector<client_direction_update>& client_direction_updates,
    const std::vector<client_groupability_update>& client_groupability_updates) {
    refreshPlayers(client_ids);
    updatePlayers(client_direction_updates, client_groupability_updates);
    updateGroups();
}

/**
    Updates the mapping of clients to players and the active status of players.
*/
void GameController::refreshPlayers(std::vector<int> client_ids) {
    // Reset players
    for (int player_id : mLevelController->getPlayerIds()) {
        mLevelController->setPlayerActive(player_id, false);
    }

    // Create new players for clients without a player
    for (const int client_id : client_ids) {
        if (mClientToPlayer.find(client_id) == mClientToPlayer.end()) {
            // Client doesn't have player
            mClientToPlayer[client_id] = assignPlayer();
        } else {
            mLevelController->setPlayerActive(mClientToPlayer[client_id], true);
        }
    }
}

/**
    Updates the properties of players based on updates recieved from the clients.
*/
void GameController::updatePlayers(std::vector<client_direction_update> client_direction_updates,
        std::vector<client_groupability_update> client_groupability_updates
    ) {
    std::vector<std::shared_ptr<Player>> players = mLevelController->getPlayers();
    // Update player directions
    for (const auto& client_direction_update : client_direction_updates) {
        sf::Uint32 client_id = client_direction_update.client_id;
        players[mClientToPlayer[client_id]]->setDirection(
                sf::Vector2f(client_direction_update.x_dir, client_direction_update.y_dir));
    }

    for (const auto& client_groupability_update : client_groupability_updates) {
        sf::Uint32 client_id = client_groupability_update.client_id;
        players[mClientToPlayer[client_id]]->setGroupable(client_groupability_update.groupable);
    }
}

/**
    Updates the active status and properties of groups.
*/
void GameController::updateGroups() {
    for (auto group : mLevelController->getGroups()) {
        group->update();
    }
}

void GameController::updateGameObjectsPostPhysics() {
    for (auto group : mLevelController->getGroups()) {
        group->matchRigid();
    }

    for (auto mine : mLevelController->getMines()) {
        mine->matchRigid();
    }
}

void GameController::setNetworkState() {
    std::vector<std::shared_ptr<Group>> active_groups = mLevelController->getActiveGroups();
    std::vector<std::shared_ptr<Mine>> active_mines = mLevelController->getActiveMines();
    mNetworkingServer->setState(active_groups, active_mines);
}

unsigned int GameController::assignPlayer() {
    int new_player_id = mLevelController->createPlayer();
    mLevelController->createGroup(new_player_id);

    return new_player_id;
}
