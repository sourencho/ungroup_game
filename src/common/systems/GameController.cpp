#include "GameController.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "../events/EventController.hpp"
#include "../factories/IdFactory.hpp"
#include "../util/game_def.hpp"
#include "../util/game_settings.hpp"

GameController::GameController() :
    m_physicsController(new PhysicsController()), m_resourceStore(new ResourceStore()),
    m_gameObjectController(new GameObjectController(*m_physicsController, *m_resourceStore)) {
}

GameController::~GameController() {
}

void GameController::step() {
    preUpdate();

    InputDef::PlayerInputs pi = getPlayerInputs();

    // Take a variable amount of game state steps depending on how long the last frame took. See:
    // https://web.archive.org/web/20190403012130/https://gafferongames.com/post/fix_your_timestep/
    sf::Int32 frame_time = m_clock.restart().asMilliseconds();
    m_timeAccumulator += frame_time;
    while (m_timeAccumulator >= MIN_TIME_STEP) {
        update(pi, MIN_TIME_STEP);

        // Clear updates since they've already been consumed
        pi.player_reliable_inputs.clear();
        pi.player_unreliable_inputs.clear();

        m_timeAccumulator -= MIN_TIME_STEP;
        m_elapsedTime += MIN_TIME_STEP;
    }

    postUpdate();
}

void GameController::computeGameState(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) {
    EventController::getInstance().forceProcessEvents();
    m_gameObjectController->update(pi);
    m_physicsController->update(delta_ms);
    m_gameObjectController->updatePostPhysics();
    incrementTick();
}
