#include <stdio.h>
#include <iostream>
#include "Client.hpp"

Client::Client(int max_player_count, sf::Keyboard::Key keys[4]):mDirection(1.0, 1.0) {
    for (int i=0; i < max_player_count; i++) {
        mGroupShapes.push_back(
            new GroupShape(
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
    // TODO: assign id from client and use it to create player
    mNetworkingClient = new NetworkingClient();
    mNetworkingClient->Start();
}

Client::~Client() {
    //dtor
}

void Client::draw(sf::RenderTarget& target) {
    // Draw active groupShapes
    for (auto group_shape: mGroupShapes) {
        if (group_shape->isActive()) {
            group_shape->draw(target);
        }
    }
}

void Client::update() {
    std::vector<network_game_object> network_game_objects = mNetworkingClient->getNetworkGameObjects();

    // Network update state
    for (auto group_shape : mGroupShapes) {
        group_shape->setActive(false);
    }
    for(auto network_game_object: network_game_objects) {
        int active_group_id = network_game_object.id;
        if (active_group_id >= mGroupShapes.size()) {
            throw std::runtime_error("Update group with no corresponding GroupShape");
        }

        GroupShape* group_shape = mGroupShapes[active_group_id];

        group_shape->setPosition(sf::Vector2f(network_game_object.x_pos, network_game_object.y_pos));
        group_shape->setRadius(network_game_object.size);
        group_shape->setActive(true);
    }

    // Network update direction
    mNetworkingClient->setDirection(mDirection);
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
