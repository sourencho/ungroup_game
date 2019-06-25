#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "GameController.hpp"

GameController::GameController(int max_player_count) {
    // Initialize Players
    for (int i=0; i < max_player_count; i++) {
        mPlayers.push_back(std::shared_ptr<Player>(new Player()));
    }

    // Initialize Groups
    for (int i=0; i < max_player_count; i++) {
        mGroups.push_back(std::shared_ptr<Group>(new Group(i, sf::Vector2f(20.f * i, 0.f))));
    }

    // Start Network Server
    mNetworkingServer = std::unique_ptr<NetworkingServer>(new NetworkingServer());
}

GameController::~GameController() {}

void GameController::update() {
    client_inputs cis = collectInputs();
    computeGameState(cis.client_ids, cis.client_direction_updates);
    setNetworkState();
    incrementTick();
}

client_inputs GameController::collectInputs() {
    return mNetworkingServer->collectClientInputs();
}

void GameController::computeGameState(
    std::vector<int>& client_ids,
    std::vector<client_direction_update>& client_direction_updates
) {
    refreshPlayers(client_ids);
    updatePlayers(client_direction_updates);
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
void GameController::refreshAndUpdateGroups() {
    // Update groups
    for(auto group: mGroups) {
        group->update();
    }

    // Detect and handle group collisions
    Group::handleCollisions(mGroups);
}

void GameController::setNetworkState() {
    mNetworkingServer->setState(getActiveGroups());
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

std::vector<std::shared_ptr<Group>> GameController::getActiveGroups() {
    std::vector<std::shared_ptr<Group>> active_groups;
    std::copy_if(
        mGroups.begin(), mGroups.end(), std::back_inserter(active_groups),
        [](std::shared_ptr<Group> group){return group->isActive();}
    );
    return active_groups;
}
