#include "GroupController.hpp"

#include <algorithm>
#include <exception>
#include <numeric>

#include "../events/ClientDisconnectedEvent.hpp"
#include "../events/CollisionEvent.hpp"
#include "../events/EventController.hpp"
#include "../factories/IdFactory.hpp"
#include "../rendering/RenderingDef.hpp"
#include "../util/game_def.hpp"
#include "../util/game_settings.hpp"

GroupController::GroupController(std::vector<std::shared_ptr<Group>>& groups,
                                 std::vector<std::shared_ptr<Player>>& players)
    : mPlayers(players), mGroups(groups) {
    addEventListeners();
}

void GroupController::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_COLLISION,
        std::bind(&GroupController::handleCollisionEvent, this, std::placeholders::_1));

    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_CLIENT_DISCONNECTED,
        std::bind(&GroupController::handleClientDisconnectedEvent, this, std::placeholders::_1));
}

uint32_t GroupController::createGroup(uint32_t player_id) {
    size_t next_group_index = nextGroupIndex++;
    if (next_group_index >= mGroups.size()) {
        throw std::out_of_range("Exceeded max number of groups.");
    }

    Group& new_group = *mGroups[next_group_index];
    uint32_t new_group_id = new_group.getId();
    mGroupToPlayers[new_group_id].push_back(player_id);
    mPlayerToGroup[player_id] = new_group_id;
    return new_group_id;
}

void GroupController::draw(sf::RenderTarget& target) {
    for (auto& group : mGroups) {
        group->draw(target);
    }
}

void GroupController::update() {
    for (auto& group : mGroups) {
        refreshGroup(group);
        updateGroup(group);
    }
}

void GroupController::updatePostPhysics() {
    for (auto& group : mGroups) {
        group->matchRigid();
    }
}

/**
 * Updates the active status of the group.
 */
void GroupController::refreshGroup(std::shared_ptr<Group>& group) {
    auto& group_players = mGroupToPlayers[group->getId()];

    bool any_active_members =
        std::any_of(group_players.begin(), group_players.end(),
                    [this](int player_id) { return getPlayer(player_id).isActive(); });
    if (any_active_members) {
        group->setActive(true);
    } else {
        group->setActive(false);
    }
}

/**
 * Updates the properties of the group
 */
void GroupController::updateGroup(std::shared_ptr<Group>& group) {
    if (!group->isActive())
        return;

    auto& group_players = mGroupToPlayers[group->getId()];

    // Group's velocity is an accumilation of it's members velocities
    sf::Vector2f group_dir =
        std::accumulate(group_players.begin(), group_players.end(), sf::Vector2f(0.f, 0.f),
                        [this](sf::Vector2f curr_vel, int player_id) {
                            return curr_vel + getPlayer(player_id).getDirection();
                        });

    group->applyInput(group_dir);

    // Group is joinable if any member player wants to group
    // TODO(sourenp): Should probably switch to voting functionality later
    bool joinable = std::accumulate(
        group_players.begin(), group_players.end(), false,
        [this](bool curr, int player_id) { return curr || getPlayer(player_id).getJoinable(); });

    group->setJoinable(joinable);

    // Update group size
    group->setRadius(group_players.size() * GROUP_MEMBER_SIZE);
    group->setMass(group_players.size() / 10.f);
}

GroupControllerUpdate GroupController::getUpdate() {
    std::vector<GroupIdAndPlayerIds> group_id_and_player_idss;

    for (auto kv : mGroupToPlayers) {
        if (kv.second.size()) {
            GroupIdAndPlayerIds group_id_and_player_ids;
            group_id_and_player_ids.group_id = kv.first;
            group_id_and_player_ids.player_ids_size = kv.second.size();
            group_id_and_player_ids.player_ids = kv.second;
            group_id_and_player_idss.push_back(group_id_and_player_ids);
        }
    }

    sf::Uint32 group_id_and_player_ids_size = group_id_and_player_idss.size();
    GroupControllerUpdate gcu = {group_id_and_player_ids_size, group_id_and_player_idss};

    return gcu;
}

void GroupController::applyUpdate(GroupControllerUpdate gcu) {
    mGroupToPlayers.clear();
    mPlayerToGroup.clear();
    for (auto& gipi : gcu.group_id_and_player_idss) {
        mGroupToPlayers[gipi.group_id] = gipi.player_ids;
    }
    for (auto& gipi : gcu.group_id_and_player_idss) {
        for (auto& player_id : gipi.player_ids) {
            mPlayerToGroup[player_id] = gipi.group_id;
        }
    }
}

uint32_t GroupController::getGroupId(uint32_t player_id) { return mPlayerToGroup[player_id]; }

Group& GroupController::getGroup(uint32_t group_id) {
    return *mGroups[IdFactory::getInstance().getIndex(group_id)];
}

Player& GroupController::getPlayer(uint32_t player_id) {
    return *mPlayers[IdFactory::getInstance().getIndex(player_id)];
}

void GroupController::handleCollisionEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<CollisionEvent> collision_event =
        std::dynamic_pointer_cast<CollisionEvent>(event);
    Collision collision = collision_event->getCollision();
    joinGroups(collision.ids.first, collision.ids.second);
}

void GroupController::joinGroups(uint32_t circle_a_id, uint32_t circle_b_id) {
    if (IdFactory::getInstance().getType(circle_a_id) != GameObjectType::group ||
        IdFactory::getInstance().getType(circle_b_id) != GameObjectType::group) {
        return;
    }

    Group& group_a = getGroup(circle_a_id);
    Group& group_b = getGroup(circle_b_id);

    if (!group_a.getJoinable() || !group_b.getJoinable()) {
        return;
    }

    // Transfer players from smaller group to larger
    uint32_t from_group_id = circle_a_id;
    uint32_t to_group_id = circle_b_id;
    if (mGroupToPlayers[circle_a_id].size() > mGroupToPlayers[circle_b_id].size()) {
        from_group_id = circle_b_id;
        to_group_id = circle_a_id;
    }

    mGroupToPlayers[to_group_id].insert(
        mGroupToPlayers[to_group_id].end(),
        std::make_move_iterator(mGroupToPlayers[from_group_id].begin()),
        std::make_move_iterator(mGroupToPlayers[from_group_id].end()));
    mGroupToPlayers[from_group_id].clear();

    for (auto& player_id : mGroupToPlayers[to_group_id]) {
        mPlayerToGroup[player_id] = to_group_id;
    }
}

void GroupController::handleClientDisconnectedEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<ClientDisconnectedEvent> client_disconnect_event =
        std::dynamic_pointer_cast<ClientDisconnectedEvent>(event);
    uint32_t player_id = client_disconnect_event->getPlayerId();
    removePlayer(player_id);
}

void GroupController::removePlayer(uint32_t player_id) {
    uint32_t group_id = mPlayerToGroup[player_id];
    auto& group_players = mGroupToPlayers[group_id];
    group_players.erase(std::remove(group_players.begin(), group_players.end(), player_id),
                        group_players.end());
    mPlayerToGroup.erase(player_id);
}

/* Network utilities */

sf::Packet& operator<<(sf::Packet& packet, const GroupIdAndPlayerIds& gipi) {
    packet << gipi.group_id;
    packet << gipi.player_ids_size;

    for (auto& player_id : gipi.player_ids) {
        packet << player_id;
    }

    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, GroupIdAndPlayerIds& gipi) {
    packet >> gipi.group_id;
    packet >> gipi.player_ids_size;

    std::vector<sf::Uint32> player_ids;
    player_ids.reserve(gipi.player_ids_size);
    for (int i = 0; i < gipi.player_ids_size; ++i) {
        sf::Uint32 player_id;
        packet >> player_id;
        player_ids.push_back(player_id);
    }
    gipi.player_ids = player_ids;

    return packet;
}

sf::Packet& operator<<(sf::Packet& packet, const GroupControllerUpdate& gcu) {
    packet << gcu.group_id_and_player_ids_size;
    for (auto& group_id_and_player_ids : gcu.group_id_and_player_idss) {
        packet << group_id_and_player_ids;
    }

    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, GroupControllerUpdate& gcu) {
    packet >> gcu.group_id_and_player_ids_size;

    std::vector<GroupIdAndPlayerIds> group_id_and_player_idss;
    group_id_and_player_idss.reserve(gcu.group_id_and_player_ids_size);
    for (int i = 0; i < gcu.group_id_and_player_ids_size; ++i) {
        GroupIdAndPlayerIds group_id_and_player_ids;
        packet >> group_id_and_player_ids;
        group_id_and_player_idss.push_back(group_id_and_player_ids);
    }
    gcu.group_id_and_player_idss = group_id_and_player_idss;

    return packet;
}
