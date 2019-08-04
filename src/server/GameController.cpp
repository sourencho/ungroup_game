#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <memory>

#include "GameController.hpp"
#include "../common/game_settings.hpp"


GameController::GameController(size_t max_player_count, size_t max_mine_count)
    :mNetworkingServer(new NetworkingServer()), mPhysicsController(new PhysicsController()) {
    loadLevel(max_player_count, max_mine_count);
}

GameController::~GameController() {}

void GameController::loadLevel(size_t max_player_count, size_t max_mine_count) {
    mPlayers.reserve(max_player_count);
    mGroups.reserve(max_player_count);
    mMines.reserve(max_mine_count);

    // Initialize Players
    for (int i=0; i < max_player_count; i++) {
        mPlayers.push_back(std::shared_ptr<Player>(new Player()));
    }

    // Initialize Groups
    for (int i=0; i < max_player_count; i++) {
        mGroups.push_back(std::shared_ptr<Group>(new Group(
            i,
            sf::Vector2f(GROUP_START_OFFSET_X * (i+1), GROUP_START_OFFSET_Y),
            mPhysicsController->createCRB())));
    }

    // Initialize Mines
    for (int i=0; i < max_mine_count; i++) {
        std::shared_ptr<Mine> new_mine = std::shared_ptr<Mine>(new Mine(
            i,
            sf::Vector2f(MINE_START_OFFSET_X, MINE_START_OFFSET_Y * (i+1)),
            MINE_SIZE,
            mPhysicsController->createCRB()));
        new_mine->setActive(true);
        mMines.push_back(new_mine);
    }
}

void GameController::update() {
    client_inputs cis = collectInputs();
    computeGameState(cis.client_ids, cis.client_direction_updates);
    setNetworkState();
    incrementTick();
}

void GameController::computeGameState(
    const std::vector<int>& client_ids,
    const std::vector<client_direction_update>& client_direction_updates
) {
    updateGameObjects(client_ids, client_direction_updates);
    mPhysicsController->step();
    mPhysicsController->handleCollision();
    matchRigid();
}

client_inputs GameController::collectInputs() {
    return mNetworkingServer->collectClientInputs();
}

void GameController::incrementTick() {
    mNetworkingServer->incrementTick();
}

void GameController::updateGameObjects(
    const std::vector<int>& client_ids,
    const std::vector<client_direction_update>& client_direction_updates) {
    refreshPlayers(client_ids);
    updatePlayers(client_direction_updates);
    updateGroups();
}

/**
    Updates the mapping of clients to players and the active status of players.
*/
void GameController::refreshPlayers(std::vector<int> client_ids) {
    // Reset players
    for (std::shared_ptr<Player> player : mPlayers) {
        player->setActive(false);
    }

    // Create new players for clients without a player
    for (const int client_id : client_ids) {
        if (mClientToPlayer.find(client_id) == mClientToPlayer.end()) {
            // Client doesn't have player
            mClientToPlayer[client_id] = assignPlayer();
        } else {
            mPlayers[mClientToPlayer[client_id]]->setActive(true);
        }
    }
}

/**
    Updates the properties of players based on updates recieved from the clients.
*/
void GameController::updatePlayers(std::vector<client_direction_update> client_direction_updates) {
    // Update player directions
    for (const auto& client_direction_update : client_direction_updates) {
        sf::Uint32 client_id = client_direction_update.client_id;
        mPlayers[mClientToPlayer[client_id]]->setDirection(
                sf::Vector2f(client_direction_update.x_dir, client_direction_update.y_dir));
    }
}

/**
    Updates the active status and properties of groups.
*/
void GameController::updateGroups() {
    // Update groups
    for (auto group : mGroups) {
        group->update();
    }
}

void GameController::matchRigid() {
    for (auto group : mGroups) {
        group->matchRigid();
    }

    for (auto mine : mMines) {
        mine->matchRigid();
    }
}

void GameController::setNetworkState() {
    mNetworkingServer->setState(Group::getActiveGroups(mGroups), Mine::getActiveMines(mMines));
}

unsigned int GameController::assignPlayer() {
    int new_player_id = mNextPlayerId++;
    if (new_player_id >= mPlayers.size() || new_player_id >= mGroups.size()) {
        throw std::runtime_error("Create players or groups with id out of range");
    }

    mPlayers[new_player_id]->setActive(true);
    std::cout << "Created Player " << new_player_id << std::endl;

    // Create group for player
    mGroups[new_player_id]->setActive(true);
    mGroups[new_player_id]->addMember(mPlayers[new_player_id]);

    return new_player_id;
}
