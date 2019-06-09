#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "GameController.hpp"

GameController::GameController(int max_player_count) {
    // Initialize Players
    for (int i=0; i < max_player_count; i++) {
        mPlayers.push_back(new Player());
    }

    // Initialize Groups
    for (int i=0; i < max_player_count; i++) {
        mGroups.push_back(new Group(i, sf::Vector2f(20.f * i, 20.f * i)));
    }

    // Start Network Server
    mNetworkingServer = new NetworkingServer();
    mNetworkingServer->Start();
}

GameController::~GameController() {}

void GameController::update() {
    // Get client input
    getClientInput();
    std::vector<int> client_ids = mNetworkingServer->getClientIds();
    std::vector<client_direction_update> client_direction_updates = \
        mNetworkingServer->getClientDirectionUpdates();

    // Update State
    updateClientPlayers(client_ids);
    updatePlayers(client_direction_updates);
    updateGroups();
    updateNetworkState();

    // Tick++
    incrementTick();
}

void GameController::getClientInput() {
    mNetworkingServer->getClientInput();
}

void GameController::incrementTick() {
    mNetworkingServer->incrementTick();
}

void GameController::updateClientPlayers(std::vector<int> client_ids) {
    // Reset players
    for (Player* player : mPlayers) {
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

void GameController::updatePlayers(std::vector<client_direction_update> client_direction_updates) {
    // Update player directions
    for (const auto& client_direction_update : client_direction_updates) {
        sf::Uint32 client_id = client_direction_update.client_id;
        mPlayers[mClientToPlayer[client_id]]->setDirection(
                sf::Vector2f(client_direction_update.x_dir, client_direction_update.y_dir));
    }
}

void GameController::updateGroups() {
    // Update groups
    for(auto group: mGroups) {
        group->update();
    }

    // Detect and handle group collisions
    Group::handleCollisions(mGroups);
}

void GameController::updateNetworkState() {
    mNetworkingServer->setState(getActiveGroups());
}

int GameController::createPlayer() {
    int new_player_id = mNextPlayerId++;
    if (new_player_id >= mPlayers.size() || new_player_id >= mGroups.size()) {
        throw std::runtime_error("Create players or groups with id out of range");
    }

    mPlayers[new_player_id]->setActive(true);
    std::cout << "Created player " << new_player_id << std::endl;

    // Create group for player
    mGroups[new_player_id]->setActive(true);
    mGroups[new_player_id]->addMember(mPlayers[new_player_id]);
    std::cout << "Created group " << new_player_id << std::endl;
    return new_player_id;
}

std::vector<Group*> GameController::getActiveGroups() {
    std::vector<Group*> active_groups;
    for(auto group: mGroups) {
        if (group->isActive()) {
            active_groups.push_back(group);
        }
    }
    return active_groups;
}
