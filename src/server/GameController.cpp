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

void GameController::updatePlayers(const ClientInputs& cis) {
    for (const auto& new_client_id : cis.new_client_ids) {
        int new_player_id = mClientToPlayer[new_client_id] = createPlayerWithGroup();
        mNetworkingServer->setClientToPlayerId(new_client_id, new_player_id);
    }

    for (const auto& removed_client_id : cis.removed_client_ids) {
        mLevelController->setPlayerActive(mClientToPlayer[removed_client_id], false);
    }

    for (const auto& client_id_and_udp_update : cis.client_id_and_udp_updates) {
        int player_id = mClientToPlayer[client_id_and_udp_update.client_id];
        std::shared_ptr<Player> player = mLevelController->getPlayer(player_id);
        player->setDirection(client_id_and_udp_update.client_udp_update.direction);
    }
    for (const auto& client_id_and_tcp_update : cis.client_id_and_tcp_updates) {
        int player_id = mClientToPlayer[client_id_and_tcp_update.client_id];
        std::shared_ptr<Player> player = mLevelController->getPlayer(player_id);
        player->setGroupable(client_id_and_tcp_update.client_tcp_update.groupable);
    }
}

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
