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
    ClientInputs cis = collectInputs();
    computeGameState(cis);
    setNetworkState();
    incrementTick();
}

ClientInputs GameController::collectInputs() {
    return mNetworkingServer->collectClientInputs();
}

void GameController::computeGameState(const ClientInputs& cis) {
    updateGameObjects(cis);
    mPhysicsController->step();
    mPhysicsController->handleCollision();
    updateGameObjectsPostPhysics();
}


void GameController::updateGameObjects(const ClientInputs& cis) {
    updatePlayers(cis);
    updateGroups();
}

/**
    Updates the active status and  properties of players based on updates recieved from the clients.
*/
void GameController::updatePlayers(const ClientInputs& cis) {
    for (const auto& new_client_id : cis.new_client_ids) {
        int new_player_id = mClientToPlayer[new_client_id] = createPlayerWithGroup();
        mNetworkingServer->setClientToPlayerId(new_client_id, new_player_id);
    }

    for (const auto& removed_client_id : cis.removed_client_ids) {
        mLevelController->setPlayerActive(mClientToPlayer[removed_client_id], false);
    }

    for (const auto& player_update : cis.player_updates) {
        mLevelController->getPlayer(player_update.player_id)->applyUpdate(player_update);
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
    std::vector<std::shared_ptr<Group>> groups = mLevelController->getGroups();
    std::vector<std::shared_ptr<Mine>> mines = mLevelController->getMines();
    mNetworkingServer->setState(groups, mines);
}

unsigned int GameController::createPlayerWithGroup() {
    int new_player_id = mLevelController->createPlayer();
    mLevelController->createGroup(new_player_id);
    return new_player_id;
}

void GameController::incrementTick() {
    mNetworkingServer->incrementTick();
}
