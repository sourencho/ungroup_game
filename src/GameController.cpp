#include "GameController.hpp"
#include <iostream>
#include <cstdlib>

GameController::GameController() {
    mCollisionDetector = new CollisionDetector();
    mNextGroupId = 0;
}

GameController::~GameController() {
    //de-construcstructor
}

void GameController::update() {
    // Update players
    for(auto player: mPlayers) {
        player->update();
    }
    
    // Update groups
    for(auto group: mGroups) {
        group->update();
    }
    
    // Detect Collisions
    mCollisionDetector->detectGroupCollisions(mGroups);
}

void GameController::createPlayer(sf::Keyboard::Key keys[4]) {
    Player* new_player = new Player(keys);
    Group* new_group = new Group(mNextGroupId, sf::Vector2f(20.f * mNextGroupId, 20.f * mNextGroupId));
    mNextGroupId++;
    
    new_group->addMemeber(new_player);
    
    mPlayers.push_back(new_player);
    mGroups.push_back(new_group);
    
}

const std::vector<Group*> &GameController::getGroups() {
    return mGroups;
}

void GameController::handleEvents(sf::Event& event) {
    // Handle players
    for(auto player: mPlayers) {
        player->handleEvents(event);
    }
}
