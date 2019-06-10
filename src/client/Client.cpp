#include <stdio.h>
#include <iostream>
#include "Client.hpp"

Client::Client(int max_player_count, sf::Keyboard::Key keys[4]):mDirection(1.0, 1.0) {
    for (int i=0; i < max_player_count; i++) {
        mGroupCircles.push_back(
            new GroupCircle(
                1.f,
                sf::Vector2f(10.f, 10.f)
            )
        );
    }

    // Set up input
    mKeys.up = keys[0];
    mKeys.down = keys[1];
    mKeys.right = keys[2];
    mKeys.left = keys[3];

    // Networking
    mNetworkingClient = new NetworkingClient();
    mNetworkingClient->Start();
}

Client::~Client() {
    //dtor
}

void Client::draw(sf::RenderTarget& target) {
    // Draw active groupShapes
    for (auto group_circle: mGroupCircles) {
        if (group_circle->isActive()) {
            group_circle->getCircle()->draw(target);
        }
    }
}

void Client::update() {
    // Get group circle updates
    std::vector<group_circle_update> group_circle_updates = mNetworkingClient->getGroupCircleUpdates();
    std::vector<int> client_ids;
    for (const auto group_circle_update : group_circle_updates) {
        client_ids.push_back(group_circle_update.client_id);
    }

    // Update state
    refreshGroupCircles(client_ids);
    updateGroupCircles(group_circle_updates);

    // Set direction
    mNetworkingClient->setDirection(mDirection);
}

/**
    Updates the mapping of clients to group circles and the active status of group circles.
*/
void Client::refreshGroupCircles(std::vector<int> client_ids) {
    // Reset group circles
    for (auto group_circle : mGroupCircles) {
        group_circle->setActive(false);
    }

    // Update group circles
    for(const auto client_id: client_ids) {
        if (mClientToGroupCircle.find(client_id) == mClientToGroupCircle.end()) {
            // Client doesn't have group
            mClientToGroupCircle[client_id] = createGroupCircle();
        } else {
            // Client has group
            mGroupCircles[mClientToGroupCircle[client_id]]->setActive(true);
        }
    }
}

/**
    Updates the properties of group circles based on updates recieved from the server.
*/
void Client::updateGroupCircles(std::vector<group_circle_update> group_circle_updates) {
    // Update group circles
    for(const auto group_circle_update: group_circle_updates) {
        int group_circle_id = mClientToGroupCircle[group_circle_update.client_id];
        GroupCircle* group_circle = mGroupCircles[group_circle_id];

        group_circle->getCircle()->setPosition(
                sf::Vector2f(group_circle_update.x_pos, group_circle_update.y_pos));
        group_circle->getCircle()->setRadius(group_circle_update.size);
    }
}

int Client::createGroupCircle() {
    int new_group_circle_id = mNextGroupCircleId++;
    if (new_group_circle_id >= mGroupCircles.size()) {
        throw std::runtime_error("Update group with no corresponding GroupCircle");
    }
    mGroupCircles[new_group_circle_id]->setActive(true);
    return new_group_circle_id;
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
        mDirection = sf::Vector2f(0.f,0.f);
        if(sf::Keyboard::isKeyPressed(mKeys.up)) {
            mDirection += sf::Vector2f(0.f,-1.f);
        }
        if(sf::Keyboard::isKeyPressed(mKeys.down)) {
            mDirection += sf::Vector2f(0.f,1.f);
        }
        if(sf::Keyboard::isKeyPressed(mKeys.left)) {
            mDirection += sf::Vector2f(-1.f,0.f);
        }
        if(sf::Keyboard::isKeyPressed(mKeys.right)) {
            mDirection += sf::Vector2f(1.f,0.f);
        }
        mDirection = normalize(mDirection);
    }
}
