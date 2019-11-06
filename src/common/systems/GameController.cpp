#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "GameController.hpp"

#include "../events/EventController.hpp"
#include "../factories/IdFactory.hpp"
#include "../util/game_def.hpp"
#include "../util/game_settings.hpp"

GameController::GameController(size_t max_player_count, size_t max_mine_count)
    : m_physicsController(new PhysicsController()), m_resourceStore(new ResourceStore()) {
    m_gameObjectStore = std::unique_ptr<GameObjectStore>(
        new GameObjectStore(m_physicsController, *m_resourceStore));
    m_gameObjectStore->loadLevel(max_player_count, max_mine_count);

    m_playerController =
        std::unique_ptr<PlayerController>(new PlayerController(m_gameObjectStore->getPlayers()));
    m_groupController = std::unique_ptr<GroupController>(
        new GroupController(m_gameObjectStore->getGroups(), m_gameObjectStore->getPlayers()));
    m_mineController =
        std::unique_ptr<MineController>(new MineController(m_gameObjectStore->getMines()));

    m_clock.restart();

    for (int i = 0; i < max_mine_count; i++) {
        m_mineController->createMine();
    }
}

GameController::~GameController() {}

void GameController::update() {
    PlayerInputs pi = collectInputs();

    // Take a variable amount of game state steps depending on how long the last frame took. See:
    // https://web.archive.org/web/20190403012130/https://gafferongames.com/post/fix_your_timestep/
    sf::Int32 frame_time = m_clock.restart().asMilliseconds();
    m_timeAccumulator += frame_time;
    while (m_timeAccumulator >= MIN_TIME_STEP) {
        step(pi, MIN_TIME_STEP);
        m_timeAccumulator -= MIN_TIME_STEP;
        m_elapsedTime += MIN_TIME_STEP;
    }

    setNetworkState();
}

void GameController::computeGameState(const PlayerInputs& pi, sf::Int32 delta_ms) {
    updateGameObjects(pi);
    m_physicsController->update(delta_ms);
    updateGameObjectsPostPhysics();
    EventController::getInstance().forceProcessEvents();
    incrementTick();
}

void GameController::updateGameObjects(const PlayerInputs& pi) {
    m_playerController->update(pi);
    m_groupController->update();
    m_mineController->update();
}

void GameController::updateGameObjectsPostPhysics() {
    m_playerController->updatePostPhysics();
    m_groupController->updatePostPhysics();
    m_mineController->updatePostPhysics();
}

uint32_t GameController::createPlayerWithGroup(uint32_t client_id) {
    uint32_t new_player_id = m_playerController->createPlayer(client_id);
    m_groupController->createGroup(new_player_id);
    return new_player_id;
}

void GameController::applyGameState(GameState game_state) {
    setTick(static_cast<unsigned int>(game_state.tick));
    for (auto gu : game_state.group_updates) {
        m_gameObjectStore->getGroup(gu.group_id)->applyUpdate(gu);
    }
    for (auto mu : game_state.mine_updates) {
        m_gameObjectStore->getMine(mu.mine_id)->applyUpdate(mu);
    }
    for (auto pu : game_state.player_updates) {
        m_gameObjectStore->getPlayer(pu.player_id)->applyUpdate(pu);
    }
    m_groupController->applyUpdate(game_state.gcu);
}

GameState GameController::getGameState() {
    auto groups = m_gameObjectStore->getGroups();
    auto mines = m_gameObjectStore->getMines();
    auto players = m_gameObjectStore->getPlayers();
    auto gcu = m_groupController->getUpdate();

    sf::Uint32 tick = static_cast<sf::Uint32>(getTick());
    std::vector<GroupUpdate> group_updates;
    std::vector<MineUpdate> mine_updates;
    std::vector<PlayerUpdate> player_updates;
    std::transform(groups.begin(), groups.end(), std::back_inserter(group_updates),
                   [](std::shared_ptr<Group> group) { return group->getUpdate(); });
    std::transform(mines.begin(), mines.end(), std::back_inserter(mine_updates),
                   [](std::shared_ptr<Mine> mine) { return mine->getUpdate(); });
    std::transform(players.begin(), players.end(), std::back_inserter(player_updates),
                   [](std::shared_ptr<Player> player) { return player->getUpdate(); });

    GameState gs = {tick, group_updates, mine_updates, player_updates, gcu};

    return gs;
}