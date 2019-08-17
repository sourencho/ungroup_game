#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <memory>
#include "GameController.hpp"
#include "../common/collision.hpp"
#include "../common/game_settings.hpp"


GameController::GameController(size_t max_player_count, size_t max_mine_count) {
    // Initialize Players
    for (int i=0; i < max_player_count; i++) {
        mPlayers.push_back(std::shared_ptr<Player>(new Player()));
    }

    // Initialize Groups
    for (int i=0; i < max_player_count; i++) {
        mGroups.push_back(std::shared_ptr<Group>(
            new Group(i, sf::Vector2f(GROUP_START_OFFSET_X * (i+1), GROUP_START_OFFSET_Y), sf::Color(0, 255, 0))));
    }

    // Initialize Mines
    for (int i=0; i < max_mine_count; i++) {
        std::shared_ptr<Mine> new_mine = std::shared_ptr<Mine>(
            new Mine(i, sf::Vector2f(MINE_START_OFFSET_X, MINE_START_OFFSET_Y * (i+1)), MINE_SIZE, sf::Color(0, 0, 255)));
        new_mine->setActive(true);
        mMines.push_back(new_mine);
    }

    // Start Network Server
    mNetworkingServer = std::unique_ptr<NetworkingServer>(new NetworkingServer());
}

GameController::~GameController() {}

void GameController::update() {
    client_inputs cis = collectInputs();
    computeGameState(cis.client_ids, cis.client_direction_updates, cis.client_groupability_updates);
    setNetworkState();
    incrementTick();
}

client_inputs GameController::collectInputs() {
    return mNetworkingServer->collectClientInputs();
}

void GameController::computeGameState(
    const std::vector<int>& client_ids,
    const std::vector<client_direction_update>& client_direction_updates,
    const std::vector<client_groupability_update>& client_groupability_updates
) {
    refreshPlayers(client_ids);
    updatePlayers(client_direction_updates, client_groupability_updates);
    refreshAndUpdateGroups();
}

void GameController::incrementTick() {
    mNetworkingServer->incrementTick();
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
            mClientToPlayer[client_id] = createPlayer();
        } else {
            mPlayers[mClientToPlayer[client_id]]->setActive(true);
        }
    }
}

/**
    Updates the properties of players based on updates recieved from the clients.
*/
void GameController::updatePlayers(std::vector<client_direction_update> client_direction_updates,
        std::vector<client_groupability_update> client_groupability_updates
    ) {
    // Update player directions
    for (const auto& client_direction_update : client_direction_updates) {
        sf::Uint32 client_id = client_direction_update.client_id;
        mPlayers[mClientToPlayer[client_id]]->setDirection(
                sf::Vector2f(client_direction_update.x_dir, client_direction_update.y_dir));
    }

    for (const auto& client_groupability_update : client_groupability_updates) {
        sf::Uint32 client_id = client_groupability_update.client_id;
        mPlayers[mClientToPlayer[client_id]]->setGroupable(client_groupability_update.groupable);
    }
}

/**
    Updates the active status and properties of groups.
*/
void GameController::refreshAndUpdateGroups() {
    // Update groups
    for (auto group : mGroups) {
        group->update();
    }

    handleCollision();
}

void GameController::handleCollision() {
    // Get active group and mine circles
    std::vector<std::shared_ptr<Group>> active_groups = Group::getActiveGroups(mGroups);
    std::vector<std::shared_ptr<Mine>> active_mines = Mine::getActiveMines(mMines);
    std::vector<std::shared_ptr<Circle>> active_group_circles = Group::getCircles(active_groups);
    std::vector<std::shared_ptr<Circle>> active_mine_circles = Mine::getCircles(active_mines);
    std::vector<std::shared_ptr<Circle>> active_circles;

    // Concat active group and mine circles
    active_circles.reserve(active_group_circles.size() + active_mine_circles.size());
    active_circles.insert(
        active_circles.end(), active_group_circles.begin(), active_group_circles.end());
    active_circles.insert(
            active_circles.end(), active_mine_circles.begin(), active_mine_circles.end());

    // Detect and handle collision
    handle_circle_collision(active_circles);
}

void GameController::setNetworkState() {
    mNetworkingServer->setState(Group::getActiveGroups(mGroups), Mine::getActiveMines(mMines));
}

int GameController::createPlayer() {
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
