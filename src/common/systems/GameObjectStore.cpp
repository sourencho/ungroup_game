#include <memory>
#include <vector>

#include "../factories/IdFactory.hpp"
#include "../util/game_def.hpp"
#include "../util/game_settings.hpp"
#include "GameObjectStore.hpp"

GameObjectStore::GameObjectStore(PhysicsController& pc) : m_physicsController(pc) {
    m_players.reserve(MAX_PLAYER_COUNT);
    m_groups.reserve(MAX_PLAYER_COUNT);
    m_mines.reserve(MAX_MINE_COUNT);

    // Initialize Players
    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
        uint32_t new_player_id = IdFactory::getInstance().getNextId(GameObjectType::player);
        m_players.push_back(std::shared_ptr<Player>(new Player(new_player_id)));
    }

    // Initialize Groups
    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
        uint32_t new_group_id = IdFactory::getInstance().getNextId(GameObjectType::group);
        m_groups.push_back(
            std::shared_ptr<Group>(new Group(new_group_id, GAME_CENTER, m_physicsController)));
    }

    // Initialize Mines
    for (int i = 0; i < MAX_MINE_COUNT; i++) {
        uint32_t new_mine_id = IdFactory::getInstance().getNextId(GameObjectType::mine);
        m_mines.push_back(std::shared_ptr<Mine>(new Mine(new_mine_id, {0.f, 0.f}, MINE_SIZE,
                                                         ResourceType(i % RESOURCE_TYPE_COUNT),
                                                         m_physicsController)));
    }
}

GameObjectStore::~GameObjectStore() {
}

std::shared_ptr<Player>& GameObjectStore::getPlayer(uint32_t player_id) {
    return m_players[IdFactory::getInstance().getIndex(player_id)];
}

std::shared_ptr<Group>& GameObjectStore::getGroup(uint32_t group_id) {
    return m_groups[IdFactory::getInstance().getIndex(group_id)];
}

std::shared_ptr<Mine>& GameObjectStore::getMine(uint32_t mine_id) {
    return m_mines[IdFactory::getInstance().getIndex(mine_id)];
}

std::vector<std::shared_ptr<Player>>& GameObjectStore::getPlayers() {
    return m_players;
}

std::vector<std::shared_ptr<Group>>& GameObjectStore::getGroups() {
    return m_groups;
}

std::vector<std::shared_ptr<Mine>>& GameObjectStore::getMines() {
    return m_mines;
}
