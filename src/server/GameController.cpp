#include "GameController.hpp"
#include <iostream>
#include <algorithm>
#include <cstdlib>

GameController::GameController(int max_player_count) {
    for (int i=0; i < max_player_count; i++) {
        mPlayers.push_back(new Player());
    }

    for (int i=0; i < max_player_count; i++) {
        mGroups.push_back(new Group(i, sf::Vector2f(20.f * i, 20.f * i)));
    }

    mCollisionDetector = new CollisionDetector();

    mNetworkingServer = new NetworkingServer();
    mNetworkingServer->Start();
}

GameController::~GameController() {
    //de-construcstructor
}

void GameController::update() {
    // Get clients and new create players
    std::vector<int> client_ids = mNetworkingServer->getClientIds();
    for (const int client_id : client_ids) {
        if (!mPlayers[client_id]->isActive()) {
            createPlayer(client_id);
        }
    }

    // Get client positions
    std::vector<client_direction> client_directions = mNetworkingServer->getClientDirections();

    // Update player positions 
    for (const auto& client_direction : client_directions) {
        sf::Uint32 client_id = client_direction.id;
        
        mPlayers[client_id]->setDirection(sf::Vector2f(client_direction.x_dir, client_direction.y_dir));
    }

    // Update groups
    for(auto group: mGroups) {
        group->update();
    }

    // Detect Collisions
    mCollisionDetector->detectGroupCollisions(mGroups);

    // Set network state
    mNetworkingServer->setState(getActiveGroups());
}

void GameController::createPlayer(int new_player_id) {
    if (new_player_id >= mPlayers.size() || new_player_id >= mGroups.size()) {
        throw std::runtime_error("Create players or groups with id out of range");
    }

    mPlayers[new_player_id]->setActive(true);
    std::cout << "Created player " << new_player_id << std::endl;

    // Create group for player
    mGroups[new_player_id]->setActive(true);
    mGroups[new_player_id]->addMemeber(mPlayers[new_player_id]);
    std::cout << "Created group " << new_player_id << std::endl;
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
