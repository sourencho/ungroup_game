#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include <numeric>

#include "Group.hpp"
#include "../util/game_settings.hpp"


sf::Packet& operator <<(sf::Packet& packet, const GroupUpdate& group_update) {
    return packet
        << group_update.group_id
        << group_update.is_active
        << group_update.x_pos
        << group_update.y_pos
        << group_update.groupable
        << group_update.radius;
}

sf::Packet& operator >>(sf::Packet& packet, GroupUpdate& group_update) {
    return packet
        >> group_update.group_id
        >> group_update.is_active
        >> group_update.x_pos
        >> group_update.y_pos
        >> group_update.groupable
        >> group_update.radius;
}

Group::Group(int id, sf::Vector2f position, sf::Color color, std::shared_ptr<PhysicsController> pc):
  CircleGameObject(id, position, 0.f, color, pc) {}

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
        [](sf::Vector2f curr_vel, std::shared_ptr<Player> player) {
            return curr_vel + player->getDirection();
        });
    setVelocity(new_velocity * GROUP_SPEED);

    // Group is groupable if any member player wants to group
    // Should probably switch to voting functionality later
    bool groupable = std::accumulate(
        mMembers.begin(),
        mMembers.end(),
        false,
        [](bool curr, std::shared_ptr<Player> player) {
            return curr || player->getGroupable();
        });

    mGroupable = groupable;
}

/**
    Sets group to active if any of its members are active.
*/
void Group::refresh() {
    bool any_active_members = std::any_of(
        mMembers.begin(), mMembers.end(),
        [](std::shared_ptr<Player> player){return player->isActive();});
    if (any_active_members) {
        setActive(true);
    } else {
        setActive(false);
    }
}

bool Group::getGroupable() {
    return mGroupable;
}

void Group::addMember(std::shared_ptr<Player> player) {
    mMembers.push_back(player);
    setRadius(mMembers.size() * GROUP_MEMBER_SIZE);
}

GroupUpdate Group::getUpdate() {
    sf::Vector2f position = getCircle()->getPosition();
    GroupUpdate gu = {(sf::Uint32) getId(), isActive(), position.x, position.y,
      getCircle()->getRadius(), getGroupable()};
    return gu;
}
