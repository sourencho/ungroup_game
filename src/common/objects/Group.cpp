#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include <numeric>

#include "Group.hpp"


const float GROUP_MEMBER_SIZE = 15.f;
const float GROUP_SPEED = 100.f;

sf::Packet& operator <<(sf::Packet& packet, const GroupUpdate& group_update) {
    packet
        << group_update.group_id
        << group_update.is_active
        << group_update.x_pos
        << group_update.y_pos
        << group_update.groupable
        << group_update.radius;

    packet << group_update.members_size;
    for (const auto player_id : group_update.members) {
        packet << player_id;
    }

    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, GroupUpdate& group_update) {
    packet
        >> group_update.group_id
        >> group_update.is_active
        >> group_update.x_pos
        >> group_update.y_pos
        >> group_update.groupable
        >> group_update.radius;

    packet >> group_update.members_size;
    std::vector<sf::Uint32> members;
    for (int i=0; i < group_update.members_size; ++i) {
        int player_id;
        packet >> player_id;
        members.push_back(player_id);
    }
    group_update.members = members;
    return packet;
}

Group::Group(int id, sf::Vector2f position, sf::Color color, std::shared_ptr<PhysicsController> pc,
  const std::vector<std::shared_ptr<Player>>& players)
  :CircleGameObject(id, position, 0.f, color, pc), mPlayers(players) {}

Group::~Group() {}

void Group::update() {
    refresh();

    if (!isActive()) {
        return;
    }

    // Update velocity
    sf::Vector2f new_velocity = std::accumulate(
        mMembers.begin(),
        mMembers.end(),
        sf::Vector2f(0.f, 0.f),
        [this](sf::Vector2f curr_vel, int player_id) {
            return curr_vel + mPlayers[player_id]->getDirection();
        });
    setVelocity(new_velocity * GROUP_SPEED);

    // Group is groupable if any member player wants to group
    // Should probably switch to voting functionality later
    bool groupable = std::accumulate(
        mMembers.begin(),
        mMembers.end(),
        false,
        [this](bool curr, int player_id) {
            return curr || mPlayers[player_id]->getGroupable();
        });

    mGroupable = groupable;
}

/**
    Sets group to active if any of its members are active.
*/
void Group::refresh() {
    bool any_active_members = std::any_of(
        mMembers.begin(), mMembers.end(),
        [this](int player_id){
        return mPlayers[player_id]->isActive();});
    if (any_active_members) {
        setActive(true);
    } else {
        setActive(false);
    }
}

bool Group::getGroupable() {
    return mGroupable;
}

void Group::setGroupable(bool groupable) {
    mGroupable = groupable;
}

void Group::addMember(int player_id) {
    mMembers.push_back(player_id);
    setRadius(mMembers.size() * GROUP_MEMBER_SIZE);
}

GroupUpdate Group::getUpdate() {
    std::vector<sf::Uint32> members;
    std::transform(
        mMembers.begin(), mMembers.end(), std::back_inserter(members),
        [](int player_id){return static_cast<sf::Uint32>(player_id);});
    sf::Vector2f position = getCircle()->getPosition();

    GroupUpdate gu = {(sf::Uint32) getId(), isActive(), position.x, position.y,
      getCircle()->getRadius(), getGroupable(), (sf::Uint32) mMembers.size(), members};

    return gu;
}

void Group::applyUpdate(GroupUpdate gu) {
    setActive(gu.is_active);
    setPosition(sf::Vector2f(gu.x_pos, gu.y_pos));
    setRadius(gu.radius);
    setGroupable(gu.groupable);
    mMembers.clear();
    std::transform(
        gu.members.begin(), gu.members.end(), std::back_inserter(mMembers),
        [](sf::Uint32 player_id){return static_cast<int>(player_id);});
}
