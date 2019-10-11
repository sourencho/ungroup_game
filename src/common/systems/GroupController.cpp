#include "GroupController.hpp"

#include <numeric>
#include "../util/game_settings.hpp"

GroupController::GroupController(std::vector<std::shared_ptr<Group>>& groups,
    std::vector<std::shared_ptr<Player>>& players): mPlayers(players), mGroups(groups) {}

GroupController::~GroupController() {}

int GroupController::createGroup(int player_id) {
    int new_group_id = player_id;
    mGroupToPlayers[new_group_id].push_back(player_id);
    return new_group_id;
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

    bool any_active_members = std::any_of(
        group_players.begin(), group_players.end(),
        [this](int player_id) {
            return mPlayers[player_id]->isActive();});
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
    if (!group->isActive()) return;

    auto& group_players = mGroupToPlayers[group->getId()];

    // Group's velocity is an accumilation of it's members velocities
    sf::Vector2f new_velocity = std::accumulate(
        group_players.begin(), group_players.end(), sf::Vector2f(0.f, 0.f),
        [this](sf::Vector2f curr_vel, int player_id) {
            return curr_vel + mPlayers[player_id]->getDirection();});
    group->setVelocity(new_velocity * GROUP_SPEED);

    // Group is groupable if any member player wants to group
    // TODO(sourenp): Should probably switch to voting functionality later
    bool groupable = std::accumulate(
        group_players.begin(), group_players.end(), false,
        [this](bool curr, int player_id) {
            return curr || mPlayers[player_id]->getGroupable();});

    group->setGroupable(groupable);

    // Update group size
    group->setRadius(group_players.size() * GROUP_MEMBER_SIZE);
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
    for (auto& gipi : gcu.group_id_and_player_idss) {
        mGroupToPlayers[gipi.group_id] = gipi.player_ids;
    }
}

/* Network utilities */

sf::Packet& operator <<(sf::Packet& packet, const GroupIdAndPlayerIds& gipi) {
    packet << gipi.group_id;
    packet << gipi.player_ids_size;

    for (auto& player_id : gipi.player_ids) {
        packet << player_id;
    }

    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, GroupIdAndPlayerIds& gipi) {
    packet >> gipi.group_id;
    packet >> gipi.player_ids_size;

    std::vector<sf::Uint32> player_ids;
    player_ids.reserve(gipi.player_ids_size);
    for (int i=0; i < gipi.player_ids_size; ++i) {
        sf::Uint32 player_id;
        packet >> player_id;
        player_ids.push_back(player_id);
    }
    gipi.player_ids = player_ids;

    return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const GroupControllerUpdate& gcu) {
    packet << gcu.group_id_and_player_ids_size;
    for (auto& group_id_and_player_ids : gcu.group_id_and_player_idss) {
        packet << group_id_and_player_ids;
    }

    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, GroupControllerUpdate& gcu) {
    packet >> gcu.group_id_and_player_ids_size;

    std::vector<GroupIdAndPlayerIds> group_id_and_player_idss;
    group_id_and_player_idss.reserve(gcu.group_id_and_player_ids_size);
    for (int i=0; i <  gcu.group_id_and_player_ids_size; ++i) {
        GroupIdAndPlayerIds group_id_and_player_ids;
        packet >> group_id_and_player_ids;
        group_id_and_player_idss.push_back(group_id_and_player_ids);
    }
    gcu.group_id_and_player_idss = group_id_and_player_idss;

    return packet;
}
