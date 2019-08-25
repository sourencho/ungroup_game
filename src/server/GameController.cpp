#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <memory>

#include "GameController.hpp"
#include "../common/game_settings.hpp"


GameController::GameController(size_t max_player_count, size_t max_mine_count):
  mNetworkingServer(new NetworkingServer()), mPhysicsController(new PhysicsController()),
  mLevelController(new LevelController(mPhysicsController)) {
    mLevelController->loadLevel(max_player_count, max_mine_count);
}

GameController::~GameController() {}

void GameController::step() {
    client_inputs cis = collectInputs();
    computeGameState(cis);
    setNetworkState();
    incrementTick();
}

void GameController::computeGameState(const client_inputs& cis) {
    updateGameObjects(cis);
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

void GameController::updateGameObjects(const client_inputs& cis) {
    updatePlayers(cis);
    updateGroups();
}

/**
    Updates the properties of players based on updates recieved from the clients.
*/
void GameController::updatePlayers(const client_inputs& cis) {
    for (const auto& new_client_id : cis.new_client_ids) {
        mClientToPlayer[new_client_id] = assignPlayer();
    }

    for (const auto& removed_client_id : cis.removed_client_ids) {
        mLevelController->setPlayerActive(mClientToPlayer[removed_client_id], false);
    }

    // Update player directions
    for (const auto& client_direction_update : cis.client_direction_updates) {
        sf::Uint32 client_id = client_direction_update.client_id;
        mLevelController->getPlayer(mClientToPlayer[client_id])->setDirection(
          sf::Vector2f(client_direction_update.x_dir, client_direction_update.y_dir));
    }

    for (const auto& client_groupability_update : cis.client_groupability_updates) {
        sf::Uint32 client_id = client_groupability_update.client_id;
        mLevelController->getPlayer(mClientToPlayer[client_id])->setGroupable(
          client_groupability_update.groupable);
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
