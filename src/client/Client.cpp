#include <stdio.h>
#include <vector>
#include <iostream>
#include "Client.hpp"

Client::Client(int max_player_count, int max_mine_count, sf::Keyboard::Key keys[5]):
  mDirection(1.0, 1.0), mPhysicsController(new PhysicsController()),
  mNetworkingClient(new NetworkingClient()) {
    for (int i=0; i < max_player_count; i++) {
        mClientGroups.push_back(
            new ClientGroup(sf::Vector2f(10.f, 10.f), sf::Color(0, 255, 0), mPhysicsController));
    }

    for (int i=0; i < max_mine_count; i++) {
        mClientMines.push_back(
            new ClientMine(sf::Vector2f(10.f, 10.f), sf::Color(0, 0, 255), mPhysicsController));
    }

    // Set up input
    mKeys.up = keys[0];
    mKeys.down = keys[1];
    mKeys.right = keys[2];
    mKeys.left = keys[3];
    mKeys.group = keys[4];
}

Client::~Client() {}

void Client::draw(sf::RenderTarget& target, sf::Shader* shader, bool use_shader) {
    // Draw active client groups
    for (auto client_group : mClientGroups) {
        if (client_group->isActive()) {
            bool groupable = client_group->getGroupable();
            std::shared_ptr<Circle> circle = client_group->getCircle();
            if (groupable) {
                circle->changeColor(sf::Color(255, 0, 0));
            } else {
                circle->setColor();
            }
            circle->draw(target, shader, use_shader);
        }
    }

    // Draw active client mines
    for (auto client_mine : mClientMines) {
        if (client_mine->isActive()) {
            client_mine->getCircle()->draw(target, shader, use_shader);
        }
    }
}

void Client::update() {
    // Get group updates
    std::vector<GroupUpdate> group_updates = mNetworkingClient->getGroupUpdates();
    std::vector<int> group_ids;
    std::transform(
        group_updates.begin(), group_updates.end(), std::back_inserter(group_ids),
        [](GroupUpdate gu){return gu.group_id;});

    // Get mine updates
    std::vector<MineUpdate> mine_updates = mNetworkingClient->getMineUpdates();
    std::vector<int> mine_ids;
    std::transform(
        mine_updates.begin(), mine_updates.end(), std::back_inserter(mine_ids),
        [](MineUpdate mu){return mu.mine_id;});

    // Update state
    refreshClientGroups(group_ids);
    updateClientGroups(group_updates);
    refreshClientMines(mine_ids);
    updateClientMines(mine_updates);

    // Set direction
    mNetworkingClient->setDirection(mDirection);
    mNetworkingClient->setGroupable(mGroupable);
}

/**
    Updates the active status of client groups and the mapping of group ids to client groups.
*/
void Client::refreshClientGroups(std::vector<int> group_ids) {
    for (auto client_group : mClientGroups) {
        client_group->setActive(false);
    }
    for (const auto group_id : group_ids) {
        if (mGroupIdToClientGroup.find(group_id) == mGroupIdToClientGroup.end()) {
            // id doesn't have matching group
            mGroupIdToClientGroup[group_id] = createClientGroup();
        } else {
            // Client has group
            mClientGroups[mGroupIdToClientGroup[group_id]]->setActive(true);
        }
    }
}

/**
    Updates the properties of client groups based on updates received from the server.
*/
void Client::updateClientGroups(std::vector<GroupUpdate> group_updates) {
    for (const auto group_update : group_updates) {
        int client_group_id = mGroupIdToClientGroup[group_update.group_id];
        ClientGroup* client_group = mClientGroups[client_group_id];
        client_group->setPosition(sf::Vector2f(group_update.x_pos, group_update.y_pos));
        client_group->getCircle()->setRadius(group_update.radius);
        client_group->setGroupable(group_update.groupable);
    }
}

/**
    Updates the active status of client mines and the mapping of mine ids to client mines.
*/
void Client::refreshClientMines(std::vector<int> mine_ids) {
    for (auto client_mine : mClientMines) {
        client_mine->setActive(false);
    }
    for (const auto mine_id : mine_ids) {
        if (mMineIdToClientMine.find(mine_id) == mMineIdToClientMine.end()) {
            // id doesn't have matching mine
            mMineIdToClientMine[mine_id] = createClientMine();
        } else {
            // Client has mine
            mClientMines[mMineIdToClientMine[mine_id]]->setActive(true);
        }
    }
}

/**
    Updates the properties of client mines based on updates recieved from the server.
*/
void Client::updateClientMines(std::vector<MineUpdate> mine_updates) {
    for (const auto mine_update : mine_updates) {
        int client_mine_id = mMineIdToClientMine[mine_update.mine_id];
        ClientMine* client_mine = mClientMines[client_mine_id];

        client_mine->setPosition(
                sf::Vector2f(mine_update.x_pos, mine_update.y_pos));
        client_mine->getCircle()->setRadius(mine_update.radius);
    }
}

int Client::createClientGroup() {
    int new_client_group_id = mNextClientGroupId++;
    if (new_client_group_id >= mClientGroups.size()) {
        throw std::runtime_error("Update group with no corresponding ClientGroup");
    }
    mClientGroups[new_client_group_id]->setActive(true);
    return new_client_group_id;
}

int Client::createClientMine() {
    int new_client_mine_id = mNextClientMineId++;
    if (new_client_mine_id >= mClientMines.size()) {
        throw std::runtime_error("Update mine with no corresponding ClientMine");
    }
    mClientMines[new_client_mine_id]->setActive(true);
    return new_client_mine_id;
}

sf::Vector2f Client::getDirection() const {
    return mDirection;
}

void Client::setId(sf::Uint32 id) {
    mId = id;
}


sf::Uint32 Client::getId() const {
    return mId;
}

void Client::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (sf::Keyboard::isKeyPressed(mKeys.group)) {
            mGroupable ^= true;
            return;
        }
        mDirection = sf::Vector2f(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(mKeys.up)) {
            mDirection += sf::Vector2f(0.f, -1.f);
        }
        if (sf::Keyboard::isKeyPressed(mKeys.down)) {
            mDirection += sf::Vector2f(0.f, 1.f);
        }
        if (sf::Keyboard::isKeyPressed(mKeys.left)) {
            mDirection += sf::Vector2f(-1.f, 0.f);
        }
        if (sf::Keyboard::isKeyPressed(mKeys.right)) {
            mDirection += sf::Vector2f(1.f, 0.f);
        }
        mDirection = normalize(mDirection);
    }
}
