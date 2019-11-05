#include <memory>
#include <vector>

#include "../factories/IdFactory.hpp"
#include "../rendering/RenderingDef.hpp"
#include "../util/game_def.hpp"
#include "../util/game_settings.hpp"
#include "GameObjectStore.hpp"

GameObjectStore::GameObjectStore(std::shared_ptr<PhysicsController> pc, ResourceStore& rs)
    : m_PhysicsController(pc), m_ResourceStore(rs) {}

GameObjectStore::~GameObjectStore() {}

void GameObjectStore::loadLevel(size_t max_player_count, size_t max_mine_count) {
    m_Players.reserve(max_player_count);
    m_Groups.reserve(max_player_count);
    m_Mines.reserve(max_mine_count);

    // Initialize Players
    for (int i = 0; i < max_player_count; i++) {
        uint32_t new_player_id = IdFactory::getInstance().getNextId(GameObjectType::player);
        m_Players.push_back(std::shared_ptr<Player>(new Player(new_player_id)));
    }

    // Initialize Groups
    for (int i = 0; i < max_player_count; i++) {
        uint32_t new_group_id = IdFactory::getInstance().getNextId(GameObjectType::group);
        m_Groups.push_back(std::shared_ptr<Group>(new Group(
            new_group_id, sf::Vector2f(GROUP_START_OFFSET_X * (i + 1), GROUP_START_OFFSET_Y),
            sf::Color::White, m_PhysicsController, m_ResourceStore)));
    }

    // Initialize Mines
    std::vector<sf::Color> mine_colors = {RenderingDef::DARKEST_COLOR, RenderingDef::DARK_COLOR,
                                          RenderingDef::LIGHT_COLOR, RenderingDef::LIGHTEST_COLOR};
    for (int i = 0; i < max_mine_count; i++) {
        uint32_t new_mine_id = IdFactory::getInstance().getNextId(GameObjectType::mine);
        m_Mines.push_back(std::shared_ptr<Mine>(new Mine(
            new_mine_id, sf::Vector2f(MINE_START_OFFSET_X, MINE_START_OFFSET_Y * (i + 1) - 100.f),
            MINE_SIZE, mine_colors[i % mine_colors.size()], m_PhysicsController, m_ResourceStore)));
    }
}

std::shared_ptr<Player>& GameObjectStore::getPlayer(uint32_t player_id) {
    return m_Players[IdFactory::getInstance().getIndex(player_id)];
}

std::shared_ptr<Group>& GameObjectStore::getGroup(uint32_t group_id) {
    return m_Groups[IdFactory::getInstance().getIndex(group_id)];
}

std::shared_ptr<Mine>& GameObjectStore::getMine(uint32_t mine_id) {
    return m_Mines[IdFactory::getInstance().getIndex(mine_id)];
}

std::vector<std::shared_ptr<Player>>& GameObjectStore::getPlayers() { return m_Players; }

std::vector<std::shared_ptr<Group>>& GameObjectStore::getGroups() { return m_Groups; }

std::vector<std::shared_ptr<Mine>>& GameObjectStore::getMines() { return m_Mines; }
