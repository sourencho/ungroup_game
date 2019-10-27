#include "PlayerController.hpp"

#include "../factories/IdFactory.hpp"
#include "../util/game_settings.hpp"
#include <exception>

PlayerController::PlayerController(std::vector<std::shared_ptr<Player>> &players)
    : mPlayers(players) {}

PlayerController::~PlayerController() {}

uint32_t PlayerController::createPlayer(uint32_t client_id) {
    size_t next_player_index = nextPlayerIndex++;
    if (next_player_index >= mPlayers.size()) {
        throw std::out_of_range("Exceeded max number of players.");
    }

    uint32_t new_player_id = mPlayers[next_player_index]->getId();
    mClientToPlayer[client_id] = new_player_id;
    getPlayer(new_player_id)->setActive(true);
    return new_player_id;
}

void PlayerController::removePlayer(uint32_t client_id) {
    getPlayer(mClientToPlayer[client_id])->setActive(false);
}

void PlayerController::update(const ClientInputs &cis) {
    uint32_t client_id;
    for (const auto &client_id_and_unreliable_update : cis.client_id_and_unreliable_updates) {
        client_id = client_id_and_unreliable_update.client_id;
        if (mClientToPlayer.count(client_id) > 0) {
            uint32_t player_id = mClientToPlayer[client_id];
            getPlayer(player_id)->setDirection(
                client_id_and_unreliable_update.client_unreliable_update.direction);
        }
    }
    for (const auto &client_id_and_reliable_update : cis.client_id_and_reliable_updates) {
        client_id = client_id_and_reliable_update.client_id;
        if (mClientToPlayer.count(client_id) > 0) {
            uint32_t player_id = mClientToPlayer[client_id];
            getPlayer(player_id)->setGroupable(
                client_id_and_reliable_update.client_reliable_update.groupable);
        }
    }
}

void PlayerController::updatePostPhysics() {
    // noop
}

void PlayerController::setPlayerClient(uint32_t player_id, uint32_t client_id) {
    mClientToPlayer[client_id] = player_id;
}

std::shared_ptr<Player> PlayerController::getPlayer(uint32_t player_id) {
    return mPlayers[IdFactory::getInstance().getIndex(player_id)];
}