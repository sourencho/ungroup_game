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

GameController::~GameController() {}

void GameController::update() {
    // Get client input
    mNetworkingServer->setAcceptingMoveCommands(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mNetworkingServer->setAcceptingMoveCommands(false);

    // Update State
    updateState();

    // Increment tick
    mNetworkingServer->incrementTick();
}

void GameController::updateState() {
    // Get clients
    std::vector<int> client_ids = mNetworkingServer->getClientIds();

    // Create new players
    for (const int client_id : client_ids) {
        if (!mPlayers[client_id]->isActive()) {
            createPlayer(client_id);
        }
    }

    // Deactivate players without a client
    for (Player* player : mPlayers) {
        player->setActive(false);
    }
    for (const int client_id : client_ids) {
        mPlayers[client_id]->setActive(true);
    }

    // Get client directions
    std::vector<network_player> network_players = mNetworkingServer->getNetworkPlayers();

    // Update player directions
    for (const auto& network_player : network_players) {
        sf::Uint32 client_id = network_player.id;
        mPlayers[client_id]->setDirection(sf::Vector2f(network_player.x_dir, network_player.y_dir));
    }

    // Update groups
    for(auto group: mGroups) {
        group->update();
    }

    // Detect and handle group collisions
    mCollisionDetector->detectAndHandleShapeCollisions(mGroups);

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
    mGroups[new_player_id]->addMember(mPlayers[new_player_id]);
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
