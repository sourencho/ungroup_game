#include "GameObjectController.hpp"

#include "../events/ClientConnectedEvent.hpp"
#include "../events/CollisionEvent.hpp"
#include "../events/EventController.hpp"
#include "../events/PlayerCreatedEvent.hpp"
#include "../factories/IdFactory.hpp"
#include "../util/game_def.hpp"
#include "../util/game_settings.hpp"

GameObjectController::GameObjectController(GameObjectStore& gos, LevelKey level_key) :
    m_gameObjectStore(gos), m_playerController(m_gameObjectStore.getPlayers()),
    m_groupController(m_gameObjectStore.getGroups(), m_gameObjectStore.getPlayers(),
                      m_resourceController),
    m_mineController(m_gameObjectStore.getMines(), m_resourceController),
    m_levelController(m_groupController, m_mineController) {
    addEventListeners();
    m_levelController.loadLevel(level_key);
}

GameObjectController::~GameObjectController(){};

void GameObjectController::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_CLIENT_CONNECTED,
        std::bind(&GameObjectController::handleClientConnectedEvent, this, std::placeholders::_1));
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_COLLISION,
        std::bind(&GameObjectController::handleCollisionEvent, this, std::placeholders::_1));
}

void GameObjectController::handleCollisionEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<CollisionEvent> collision_event =
        std::dynamic_pointer_cast<CollisionEvent>(event);

    Collision collision = collision_event->getCollision();
    transferResources(collision.ids.first, collision.ids.second);
}

void GameObjectController::transferResources(uint32_t circle_a_id, uint32_t circle_b_id) {
    uint32_t group_id, mine_id;
    if (IdFactory::getInstance().getType(circle_a_id) == GameObjectType::group &&
        IdFactory::getInstance().getType(circle_b_id) == GameObjectType::mine) {
        group_id = circle_a_id;
        mine_id = circle_b_id;
    } else if (IdFactory::getInstance().getType(circle_a_id) == GameObjectType::mine &&
               IdFactory::getInstance().getType(circle_b_id) == GameObjectType::group) {
        group_id = circle_b_id;
        mine_id = circle_a_id;
    } else {
        return;
    }
    Mine& mine = m_mineController.getMine(mine_id);
    std::vector<uint32_t> group_player_ids = m_groupController.getGroupPlayerIds(group_id);
    for (auto player_id : group_player_ids) {
        m_resourceController.move(mine_id, player_id, mine.getResourceType(), 1);
    }
}

void GameObjectController::handleClientConnectedEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<ClientConnectedEvent> client_connect_event =
        std::dynamic_pointer_cast<ClientConnectedEvent>(event);
    uint32_t client_id = client_connect_event->getClientId();
    uint32_t new_player_id = createPlayerWithGroup(client_id);
    EventController::getInstance().forceQueueEvent(
        std::shared_ptr<PlayerCreatedEvent>(new PlayerCreatedEvent(client_id, new_player_id)));
}

void GameObjectController::update(const InputDef::PlayerInputs& pi) {
    m_playerController.update(pi);
    m_groupController.update();
    m_mineController.update();
}

uint32_t GameObjectController::createPlayerWithGroup(uint32_t client_id) {
    uint32_t new_player_id = m_playerController.createPlayer(client_id);
    m_groupController.createGroup(new_player_id);
    return new_player_id;
}

void GameObjectController::applyGameStateObject(GameStateObject game_state_object) {
    for (auto gu : game_state_object.group_updates) {
        m_gameObjectStore.getGroup(gu.group_id)->applyUpdate(gu);
    }
    for (auto mu : game_state_object.mine_updates) {
        m_gameObjectStore.getMine(mu.mine_id)->applyUpdate(mu);
    }
    for (auto pu : game_state_object.player_updates) {
        m_gameObjectStore.getPlayer(pu.player_id)->applyUpdate(pu);
    }
    m_groupController.applyUpdate(game_state_object.gcu);
    m_resourceController.applyUpdate(game_state_object.rcu);
}

GameStateObject GameObjectController::getGameStateObject() {
    auto groups = m_gameObjectStore.getGroups();
    auto mines = m_gameObjectStore.getMines();
    auto players = m_gameObjectStore.getPlayers();
    auto gcu = m_groupController.getUpdate();
    auto rcu = m_resourceController.getUpdate();

    std::vector<GroupUpdate> group_updates;
    std::vector<MineUpdate> mine_updates;
    std::vector<PlayerUpdate> player_updates;
    std::transform(groups.begin(), groups.end(), std::back_inserter(group_updates),
                   [](std::shared_ptr<Group> group) { return group->getUpdate(); });
    std::transform(mines.begin(), mines.end(), std::back_inserter(mine_updates),
                   [](std::shared_ptr<Mine> mine) { return mine->getUpdate(); });
    std::transform(players.begin(), players.end(), std::back_inserter(player_updates),
                   [](std::shared_ptr<Player> player) { return player->getUpdate(); });

    GameStateObject gs = {group_updates, mine_updates, player_updates, gcu, rcu};

    return gs;
}

sf::Vector2f GameObjectController::getPlayerPosition(uint32_t player_id) {
    return m_gameObjectStore.getGroup(m_groupController.getGroupId(player_id))->getCenter();
}

std::array<uint32_t, RESOURCE_TYPE_COUNT>
GameObjectController::getPlayerResources(uint32_t player_id) {
    return m_resourceController.get(player_id);
}

std::pair<bool, uint32_t> GameObjectController::getGameOver() {
    for (auto player_id : m_playerController.getActivePlayerIds()) {
        auto player = m_playerController.getPlayer(player_id);
        if (player->getWinCondition().satisfiesCondition(m_resourceController.get(player_id))) {
            return std::make_pair(true, player_id);
        }
    }
    return std::make_pair(false, std::numeric_limits<uint32_t>::max());
}

std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
GameObjectController::getBotMove(uint32_t bot_player_id, BotStrategy strategy) {
    auto mines = m_gameObjectStore.getMines();
    auto current_resources = m_resourceController.get(bot_player_id);
    auto bot_group_id = m_groupController.getGroupId(bot_player_id);
    Group& bot_group = m_groupController.getGroup(bot_group_id);
    auto bot_group_center = bot_group.getCenter();
    std::unordered_map<uint32_t, uint32_t> mine_id_to_resource_count;
    for (auto mine : mines) {
        auto mine_id = mine->getId();
        mine_id_to_resource_count[mine_id] =
            m_resourceController.get(mine_id, mine->getResourceType());
    }

    return m_bot.getMove(strategy, mines, current_resources, bot_group_center,
                         mine_id_to_resource_count);
}

GroupController& GameObjectController::getGroupController() {
    return m_groupController;
}

MineController& GameObjectController::getMineController() {
    return m_mineController;
}

ResourceController& GameObjectController::getResourceController() {
    return m_resourceController;
}

PlayerController& GameObjectController::getPlayerController() {
    return m_playerController;
}
