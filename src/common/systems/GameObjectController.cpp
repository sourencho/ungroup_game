#include "GameObjectController.hpp"

#include "../../common/events/ClientConnectedEvent.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/events/PlayerCreatedEvent.hpp"

GameObjectController::GameObjectController(PhysicsController& physics_controller,
                                           ResourceStore& resource_store) :
    m_gameObjectStore(physics_controller, resource_store),
    m_playerController(m_gameObjectStore.getPlayers()),
    m_groupController(m_gameObjectStore.getGroups(), m_gameObjectStore.getPlayers()),
    m_mineController(m_gameObjectStore.getMines()) {
    addEventListeners();
}

GameObjectController::~GameObjectController(){};

void GameObjectController::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_CLIENT_CONNECTED,
        std::bind(&GameObjectController::handleClientConnectedEvent, this, std::placeholders::_1));
}

void GameObjectController::handleClientConnectedEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<ClientConnectedEvent> client_connect_event =
        std::dynamic_pointer_cast<ClientConnectedEvent>(event);
    uint32_t client_id = client_connect_event->getClientId();
    uint32_t new_player_id = createPlayerWithGroup(client_id);
    EventController::getInstance().forceQueueEvent(
        std::shared_ptr<PlayerCreatedEvent>(new PlayerCreatedEvent(client_id, new_player_id)));
}

void GameObjectController::update(const PlayerInputs& pi) {
    m_playerController.update(pi);
    m_groupController.update();
    m_mineController.update();
}

void GameObjectController::updatePostPhysics() {
    m_playerController.updatePostPhysics();
    m_groupController.updatePostPhysics();
    m_mineController.updatePostPhysics();
}

uint32_t GameObjectController::createPlayerWithGroup(uint32_t client_id) {
    uint32_t new_player_id = m_playerController.createPlayer(client_id);
    m_groupController.createGroup(new_player_id);
    return new_player_id;
}

void GameObjectController::applyGameState(GameState game_state) {
    for (auto gu : game_state.group_updates) {
        m_gameObjectStore.getGroup(gu.group_id)->applyUpdate(gu);
    }
    for (auto mu : game_state.mine_updates) {
        m_gameObjectStore.getMine(mu.mine_id)->applyUpdate(mu);
    }
    for (auto pu : game_state.player_updates) {
        m_gameObjectStore.getPlayer(pu.player_id)->applyUpdate(pu);
    }
    m_groupController.applyUpdate(game_state.gcu);
}

GameState GameObjectController::getGameState(uint32_t tick) {
    auto groups = m_gameObjectStore.getGroups();
    auto mines = m_gameObjectStore.getMines();
    auto players = m_gameObjectStore.getPlayers();
    auto gcu = m_groupController.getUpdate();

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

void GameObjectController::draw(sf::RenderTexture& buffer) {
    m_groupController.draw(buffer);
    m_mineController.draw(buffer);
}

sf::Vector2f GameObjectController::getPlayerPosition(uint32_t player_id) {
    return m_gameObjectStore.getGroup(m_groupController.getGroupId(player_id))->getCenter();
}