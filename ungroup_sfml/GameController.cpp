#include "GameController.hpp"
#include <iostream>
#include <cstdlib>

GameController::GameController() {
    //constructor
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
}

void GameController::createPlayer(sf::Keyboard::Key keys[4]) {
    Player* new_player = new Player(keys);
    Group* new_group = new Group(
        10.f,
        sf::Vector2f(10.f, 10.f),
        sf::Color(rand() % 255, rand() % 255, rand() % 255)
    );
    
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

    // Handle groups
    for(auto group: mGroups) {
        group->handleEvents(event);
    }
}

void GameController::draw(sf::RenderTarget& target) {
    // Draw groups
    for(auto group: mGroups) {
        group->draw(target);
    }
}
