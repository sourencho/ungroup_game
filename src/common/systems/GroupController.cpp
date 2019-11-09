#include "GroupController.hpp"

#include <algorithm>
#include <exception>
#include <numeric>

#include "../events/ClientDisconnectedEvent.hpp"
#include "../events/CollisionEvent.hpp"
#include "../events/EventController.hpp"
#include "../factories/IdFactory.hpp"
#include "../rendering/RenderingDef.hpp"
#include "../util/TypeDef.hpp"
#include "../util/game_def.hpp"
#include "../util/game_settings.hpp"

GroupController::GroupController(std::vector<std::shared_ptr<Group>>& groups,
                                 std::vector<std::shared_ptr<Player>>& players)
    : m_players(players), m_groups(groups) {
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
    if (next_group_index >= m_groups.size()) {
        throw std::out_of_range("Exceeded max number of groups.");
    }

    Group& new_group = *m_groups[next_group_index];
    uint32_t new_group_id = new_group.getId();
    m_groupToPlayers[new_group_id].push_back(player_id);
    m_playerToGroup[player_id] = new_group_id;
    return new_group_id;
}

void GroupController::draw(sf::RenderTarget& target) {
    for (auto& group : m_groups) {
        group->draw(target);
    }
}

void GroupController::update() {
    regroup(m_groups);
    for (auto& group : m_groups) {
        refreshGroup(group);
        updateGroup(group);
    }
}

void GroupController::updatePostPhysics() {
    for (auto& group : m_groups) {
        group->matchRigid();
    }
}

/**
 * Reassign players' groups based on their ungroup property.
 * The algorithm used here:
 * 1. Get all filled group and empty groups
 * 2. Get a list of all players that have ungroup=true keeping track of their original group
 * 3. Assign each of those players a new group from the list of empty groups, updating that list as
 * you reassign. For groups with just one player that has the ungroup=true, no need to reassign.
 * 4. Reset the properties of the player and the group.
 */
void GroupController::regroup(std::vector<std::shared_ptr<Group>>& groups) {
    TypeDef::ids filled_group_ids;
    TypeDef::ids empty_group_ids;
    std::tie(filled_group_ids, empty_group_ids) = partitionGroupsByPlayerCount();

    // Get all players and their original group that need to be regrouped
    std::vector<std::pair<uint32_t, uint32_t>> regroup_player_and_group_ids;
    for (uint32_t group_id : filled_group_ids) {
        auto& group_players = m_groupToPlayers[group_id];
        for (uint32_t player_id : group_players) {
            Player& player = getPlayer(player_id);
            if (!player.getUngroup() || group_players.size() == 1) {
                continue;
            }
            regroup_player_and_group_ids.push_back(std::make_pair(player_id, group_id));
        }
    }

    // Find a new group for each player that needs to be regrouped
    for (auto& player_and_group_id : regroup_player_and_group_ids) {
        uint32_t player_id, original_group_id;
        std::tie(player_id, original_group_id) = player_and_group_id;
        Player& player = getPlayer(player_id);
        // If the original group only has that player, it doesn't need to be regrouped
        if (m_groupToPlayers[original_group_id].size() == 1) {
            player.setJoinable(false);
            player.setUngroup(false);
            continue;
        }

        // There should always be an empty group to regroup to since there are an equal number of
        // players and groups
        if (empty_group_ids.size() == 0) {
            throw std::runtime_error("No empty group to regroup to.");
        }

        // Remove player from original group
        removePlayer(player_id);

        // Transfer player to an empty group
        uint32_t new_group_id = empty_group_ids.back();
        empty_group_ids.pop_back();
        m_groupToPlayers[new_group_id].push_back(player_id);
        m_playerToGroup[player_id] = new_group_id;

        // Set new group and player properties
        player.setUngroup(false);
        player.setJoinable(false);
        Group& new_group = getGroup(new_group_id);
        Group& original_group = getGroup(original_group_id);
        // TODO(sourenp): Set new group's location somewhere near the original group
        new_group.setPosition(original_group.getPosition());
    }
}

/**
 * Updates the active status of the group.
 */
void GroupController::refreshGroup(std::shared_ptr<Group>& group) {
    auto& group_players = m_groupToPlayers[group->getId()];

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

    auto& group_players = m_groupToPlayers[group->getId()];

    // Group's velocity is an accumilation of it's members velocities
    sf::Vector2f group_dir =
        std::accumulate(group_players.begin(), group_players.end(), sf::Vector2f(0.f, 0.f),
                        [this](sf::Vector2f curr_vel, int player_id) {
                            return curr_vel + getPlayer(player_id).getDirection();
                        });

    group->applyInput(group_dir);

    // Group is joinable if any member player is joinable
    // TODO(sourenp): Should probably switch to voting functionality later
    bool joinable = std::accumulate(
        group_players.begin(), group_players.end(), false,
        [this](bool curr, int player_id) { return curr || getPlayer(player_id).getJoinable(); });
    group->setJoinable(joinable);

    // Group is ungroup if any member player is ungroup
    // TODO(sourenp): This was only included for debugging purposes. A group doesn't need to know
    // when it's in the ungroup state. Remove eventually.
    bool ungroup = std::accumulate(
        group_players.begin(), group_players.end(), false,
        [this](bool curr, int player_id) { return curr || getPlayer(player_id).getUngroup(); });
    group->setUngroup(ungroup);

    // Update group size
    group->setRadius(group_players.size() * GROUP_MEMBER_SIZE);
    group->setMass(group_players.size() / 10.f);
}

GroupControllerUpdate GroupController::getUpdate() {
    std::vector<GroupIdAndPlayerIds> group_id_and_player_idss;

    for (auto kv : m_groupToPlayers) {
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
    m_playerToGroup.clear();
    for (auto& group_and_players : m_groupToPlayers) {
        group_and_players.second.clear();
    }

    for (auto& gipi : gcu.group_id_and_player_idss) {
        m_groupToPlayers[gipi.group_id] = gipi.player_ids;
    }
    for (auto& gipi : gcu.group_id_and_player_idss) {
        for (auto& player_id : gipi.player_ids) {
            m_playerToGroup[player_id] = gipi.group_id;
        }
    }
}

uint32_t GroupController::getGroupId(uint32_t player_id) { return m_playerToGroup[player_id]; }

Group& GroupController::getGroup(uint32_t group_id) {
    return *m_groups[IdFactory::getInstance().getIndex(group_id)];
}

std::pair<TypeDef::ids, TypeDef::ids> GroupController::partitionGroupsByPlayerCount() {
    TypeDef::ids filled_group_ids;
    TypeDef::ids empty_group_ids;
    for (auto& group_and_players : m_groupToPlayers) {
        if (group_and_players.second.size() == 0) {
            empty_group_ids.push_back(group_and_players.first);
        } else {
            filled_group_ids.push_back(group_and_players.first);
        }
    }
    return std::pair<TypeDef::ids, TypeDef::ids>(filled_group_ids, empty_group_ids);
}

Player& GroupController::getPlayer(uint32_t player_id) {
    return *m_players[IdFactory::getInstance().getIndex(player_id)];
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
    if (m_groupToPlayers[circle_a_id].size() > m_groupToPlayers[circle_b_id].size()) {
        from_group_id = circle_b_id;
        to_group_id = circle_a_id;
    }

    m_groupToPlayers[to_group_id].insert(
        m_groupToPlayers[to_group_id].end(),
        std::make_move_iterator(m_groupToPlayers[from_group_id].begin()),
        std::make_move_iterator(m_groupToPlayers[from_group_id].end()));
    m_groupToPlayers[from_group_id].clear();

    for (auto& player_id : m_groupToPlayers[to_group_id]) {
        m_playerToGroup[player_id] = to_group_id;
    }
}

void GroupController::handleClientDisconnectedEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<ClientDisconnectedEvent> client_disconnect_event =
        std::dynamic_pointer_cast<ClientDisconnectedEvent>(event);
    uint32_t player_id = client_disconnect_event->getPlayerId();
    removePlayer(player_id);
}

void GroupController::removePlayer(uint32_t player_id) {
    uint32_t group_id = m_playerToGroup[player_id];
    auto& group_players = m_groupToPlayers[group_id];
    group_players.erase(std::remove(group_players.begin(), group_players.end(), player_id),
                        group_players.end());
    m_playerToGroup.erase(player_id);
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
