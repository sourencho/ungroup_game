#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <memory>

#include "GameController.hpp"

#include "../util/game_settings.hpp"
#include "../events/EventController.hpp"


GameController::GameController(size_t max_player_count, size_t max_mine_count):
  mPhysicsController(new PhysicsController()),
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

void GameController::computeGameState(const ClientInputs& cis) {
    updateGameObjects(cis);
    mPhysicsController->step();
    mPhysicsController->handleCollision();
    updateGameObjectsPostPhysics();
    EventController::getInstance().forceProcessEvents();
}


void GameController::updateGameObjects(const ClientInputs& cis) {
    updatePlayers(cis);
    updateGroups();
}

void GameController::updatePlayers(const ClientInputs& cis) {
    for (const auto& client_id_and_unreliable_update : cis.client_id_and_unreliable_updates) {
        int player_id = mClientToPlayer[client_id_and_unreliable_update.client_id];
        std::shared_ptr<Player> player = mLevelController->getPlayer(player_id);
        player->setDirection(client_id_and_unreliable_update.client_unreliable_update.direction);
    }
    for (const auto& client_id_and_reliable_update : cis.client_id_and_reliable_updates) {
        int player_id = mClientToPlayer[client_id_and_reliable_update.client_id];
        std::shared_ptr<Player> player = mLevelController->getPlayer(player_id);
        player->setGroupable(client_id_and_reliable_update.client_reliable_update.groupable);
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

unsigned int GameController::createPlayerWithGroup() {
    int new_player_id = mLevelController->createPlayer();
    mLevelController->createGroup(new_player_id);
    return new_player_id;
}

void GameController::applyGameState(GameState game_state) {
    for (auto gu : game_state.group_updates) {
        mLevelController->getGroup(gu.group_id)->applyUpdate(gu);
    }
    for (auto mu : game_state.mine_updates) {
        mLevelController->getMine(mu.mine_id)->applyUpdate(mu);
    }
    for (auto pu : game_state.player_updates) {
        mLevelController->getPlayer(pu.player_id)->applyUpdate(pu);
    }
}
