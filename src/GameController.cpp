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
}

GameController::~GameController() {
    //de-construcstructor
}

void GameController::update(int client_id, sf::Vector2f client_direction) {
    // Update players
    mPlayers[client_id]->setDirection(client_direction);

    // Update groups
    for(auto group: mGroups) {
        group->update();
    }

    // Detect Collisions
    mCollisionDetector->detectGroupCollisions(mGroups);
}

size_t GameController::createPlayer() {
    int new_group_id = mNextGroupId++;
    int new_player_id = mNextPlayerId++;

    if (new_player_id >= mPlayers.size() || new_group_id >= mGroups.size()) {
        throw std::runtime_error("Create more players or groups than max");
    }

    mPlayers[new_player_id]->setActive(true);
    mGroups[new_group_id]->setActive(true);

    mGroups[new_group_id]->addMemeber(mPlayers[new_player_id]);

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
