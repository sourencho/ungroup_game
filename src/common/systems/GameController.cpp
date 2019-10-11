#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>

#include "GameController.hpp"

#include "../util/game_settings.hpp"
#include "../events/EventController.hpp"


GameController::GameController(size_t max_player_count, size_t max_mine_count):
  mPhysicsController(new PhysicsController()),
  mGameObjectStore(new GameObjectStore(mPhysicsController)) {
    mGameObjectStore->loadLevel(max_player_count, max_mine_count);
    mGroupController = std::unique_ptr<GroupController>(
        new GroupController(mGameObjectStore->getGroups(), mGameObjectStore->getPlayers()));
    mClock.restart();
}

GameController::~GameController() {}

void GameController::update() {
    ClientInputs cis = collectInputs();

    // Take a variable amount of game state steps depending on how long the last frame took. See:
    // https://web.archive.org/web/20190403012130/https://gafferongames.com/post/fix_your_timestep/
    sf::Int32 frame_time = mClock.restart().asMilliseconds();
    mTimeAccumulator += frame_time;
    while (mTimeAccumulator >= MIN_TIME_STEP) {
        computeGameState(cis, MIN_TIME_STEP);
        mTimeAccumulator -= MIN_TIME_STEP;
        mElapsedTime += MIN_TIME_STEP;
    }

    setNetworkState();

    //  std::cout << "Steps per second: "
    //      << static_cast<float>(mStepCount)/(mElapsedTime/1000) << std::endl;
}

void GameController::computeGameState(const ClientInputs& cis, sf::Int32 delta_ms) {
    updateGameObjects(cis);
    mPhysicsController->update(delta_ms);
    updateGameObjectsPostPhysics();
    EventController::getInstance().forceProcessEvents();
    mStepCount++;
    incrementTick();
}

void GameController::updateGameObjects(const ClientInputs& cis) {
    updatePlayers(cis);
    mGroupController->update();
}

void GameController::updatePlayers(const ClientInputs& cis) {
    int client_id;
    for (const auto& client_id_and_unreliable_update : cis.client_id_and_unreliable_updates) {
        client_id = client_id_and_unreliable_update.client_id;
        if(mClientToPlayer.count(client_id) > 0) {
            int player_id = mClientToPlayer[client_id];
            std::shared_ptr<Player> player = mGameObjectStore->getPlayer(player_id);
            player->setDirection(client_id_and_unreliable_update.client_unreliable_update.direction);
        }
    }
    for (const auto& client_id_and_reliable_update : cis.client_id_and_reliable_updates) {
        client_id = client_id_and_reliable_update.client_id;
        if(mClientToPlayer.count(client_id) > 0) {
            int player_id = mClientToPlayer[client_id];
            std::shared_ptr<Player> player = mGameObjectStore->getPlayer(player_id);
            player->setGroupable(client_id_and_reliable_update.client_reliable_update.groupable);
        }
    }
}

void GameController::updateGameObjectsPostPhysics() {
    mGroupController->updatePostPhysics();

    for (auto mine : mGameObjectStore->getMines()) {
        mine->matchRigid();
    }
}

unsigned int GameController::createPlayerWithGroup() {
    int new_player_id = mGameObjectStore->createPlayer();
    mGroupController->createGroup(new_player_id);
    return new_player_id;
}

void GameController::applyGameState(GameState game_state) {
    setTick(static_cast<unsigned int>(game_state.tick));
    for (auto gu : game_state.group_updates) {
        mGameObjectStore->getGroup(gu.group_id)->applyUpdate(gu);
    }
    for (auto mu : game_state.mine_updates) {
        mGameObjectStore->getMine(mu.mine_id)->applyUpdate(mu);
    }
    for (auto pu : game_state.player_updates) {
        mGameObjectStore->getPlayer(pu.player_id)->applyUpdate(pu);
    }
    mGroupController->applyUpdate(game_state.gcu);
}

GameState GameController::getGameState() {
    auto groups = mGameObjectStore->getGroups();
    auto mines = mGameObjectStore->getMines();
    auto players = mGameObjectStore->getPlayers();
    auto gcu = mGroupController->getUpdate();

    sf::Uint32 tick = static_cast<sf::Uint32>(getTick());
    std::vector<GroupUpdate> group_updates;
    std::vector<MineUpdate> mine_updates;
    std::vector<PlayerUpdate> player_updates;
    std::transform(
        groups.begin(), groups.end(), std::back_inserter(group_updates),
        [](std::shared_ptr<Group> group){return group->getUpdate();});
    std::transform(
        mines.begin(), mines.end(), std::back_inserter(mine_updates),
        [](std::shared_ptr<Mine> mine){return mine->getUpdate();});
    std::transform(
        players.begin(), players.end(), std::back_inserter(player_updates),
        [](std::shared_ptr<Player> player){return player->getUpdate();});

    GameState gs = {tick, group_updates, mine_updates, player_updates, gcu};

    return gs;
}
