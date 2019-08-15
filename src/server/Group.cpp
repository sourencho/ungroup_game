#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include <numeric>

#include "Group.hpp"
#include "../common/game_settings.hpp"

Group::Group(int id, sf::Vector2f position, std::shared_ptr<PhysicsController> pc)
    :CircleGameObject(id, position, 0.f, pc) {}

Group::~Group() {}

void Group::update() {
    refresh();

    if (isActive()) {
        // Update velocity
        sf::Vector2f new_velocity = std::accumulate(
            mMembers.begin(),
            mMembers.end(),
            sf::Vector2f(0.f, 0.f),
            [](sf::Vector2f curr_vel, std::shared_ptr<Player> player) {
                return curr_vel + player->getDirection();
            });
        setVelocity(new_velocity * GROUP_SPEED);
    }
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

void Group::addMember(std::shared_ptr<Player> player) {
    mMembers.push_back(player);
    setRadius(mMembers.size() * GROUP_MEMBER_SIZE);
}

std::vector<std::shared_ptr<Group>> Group::getActiveGroups(std::vector<std::shared_ptr<Group>>& groups) {
    std::vector<std::shared_ptr<Group>> active_groups;
    std::copy_if(
        groups.begin(), groups.end(), std::back_inserter(active_groups),
        [](std::shared_ptr<Group> group){return group->isActive();});
    return active_groups;
}
