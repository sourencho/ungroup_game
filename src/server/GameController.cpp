#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <memory>

#include "GameController.hpp"
#include "../common/game_settings.hpp"
#include "../common/events/EventController.hpp"
#include "../common/events/ClientConnectedEvent.hpp"
#include "../common/events/ClientDisconnectedEvent.hpp"


GameController::GameController(size_t max_player_count, size_t max_mine_count):
  mNetworkingServer(new NetworkingServer()), mPhysicsController(new PhysicsController()),
  mLevelController(new LevelController(mPhysicsController)) {
    mLevelController->loadLevel(max_player_count, max_mine_count);
    EventController::getInstance()->addEventListener(EventType::EVENT_TYPE_CLIENT_CONNECTED,
        std::bind(&GameController::clientConnected, this, std::placeholders::_1));
    EventController::getInstance()->addEventListener(EventType::EVENT_TYPE_CLIENT_DISCONNECTED,
        std::bind(&GameController::clientDisconnected, this, std::placeholders::_1));
}

GameController::~GameController() {}

void GameController::step() {
    ClientInputs cis = collectInputs();
    computeGameState(cis);
    setNetworkState();
    incrementTick();
}

ClientInputs GameController::collectInputs() {
    return mNetworkingServer->collectClientInputs();
}

void GameController::computeGameState(const ClientInputs& cis) {
    updateGameObjects(cis);
    mPhysicsController->step();
    mPhysicsController->handleCollision();
    updateGameObjectsPostPhysics();
    EventController::getInstance()->processEvents();
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

void GameController::setNetworkState() {
    std::vector<std::shared_ptr<Group>> groups = mLevelController->getGroups();
    std::vector<std::shared_ptr<Mine>> mines = mLevelController->getMines();
    mNetworkingServer->setState(groups, mines);
}

void GameController::clientConnected(std::shared_ptr<Event> event) {
    switch (event->getType()) {
        case EventType::EVENT_TYPE_CLIENT_CONNECTED: {
            std::shared_ptr<ClientConnectedEvent> client_connect_event = \
                std::dynamic_pointer_cast<ClientConnectedEvent>(event);
            int new_client_id = client_connect_event->getClientId();
            int new_player_id = mClientToPlayer[new_client_id] = createPlayerWithGroup();
            mNetworkingServer->setClientToPlayerId(new_client_id, new_player_id);
            break;
        }
        default: {
            std::cout << "Unexpected event type." << std::endl;
            break;
        }
    }
}


void GameController::clientDisconnected(std::shared_ptr<Event> event) {
    switch (event->getType()) {
        case EventType::EVENT_TYPE_CLIENT_DISCONNECTED: {
            std::shared_ptr<ClientDisconnectedEvent> client_disconnect_event = \
                std::dynamic_pointer_cast<ClientDisconnectedEvent>(event);
            int removed_client_id = client_disconnect_event->getClientId();
            mLevelController->setPlayerActive(mClientToPlayer[removed_client_id], false);
            break;
        }
        default: {
            std::cout << "Unexpected event type." << std::endl;
            break;
        }
    }
}

unsigned int GameController::createPlayerWithGroup() {
    int new_player_id = mLevelController->createPlayer();
    mLevelController->createGroup(new_player_id);
    return new_player_id;
}

void GameController::incrementTick() {
    mNetworkingServer->incrementTick();
}
