#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include "Client.hpp"

Client::Client(int max_player_count, int max_mine_count, sf::Keyboard::Key keys[5]):
  mPhysicsController(new PhysicsController()), mNetworkingClient(new NetworkingClient()) {
    mPlayers.reserve(max_player_count);

    for (int i=0; i < max_player_count; i++) {
        mClientGroups.push_back(
            new ClientGroup(sf::Vector2f(10.f, 10.f), sf::Color(0, 255, 0), mPhysicsController));
    }

    for (int i=0; i < max_mine_count; i++) {
        mClientMines.push_back(
            new ClientMine(sf::Vector2f(10.f, 10.f), sf::Color(0, 0, 255), mPhysicsController));
    }

    for (int i=0; i < max_player_count; i++) {
        mPlayers.push_back(new Player(i));
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
    // Get updates
    GameState game_state =  mNetworkingClient->getGameState();
    if (mPlayerId == -1) {
        mPlayerId = mNetworkingClient->getPlayerId();
    }

    // Send updates
    if (mPlayerId != -1) {
        ClientUpdate client_update = {mPlayers[mPlayerId]->getUpdate()};
        mNetworkingClient->setClientUpdate(client_update);
    }

    // Apply updates
    for (auto gu : game_state.group_updates) {
        mClientGroups[gu.group_id]->applyUpdate(gu);
    }

    for (auto mu : game_state.mine_updates) {
        mClientMines[mu.mine_id]->applyUpdate(mu);
    }
}

void Client::handleEvents(sf::Event& event) {
    if (mPlayerId == -1) {
        return;
    }
    if (event.type == sf::Event::KeyPressed) {
        // Groupable
        if (sf::Keyboard::isKeyPressed(mKeys.group)) {
            mPlayers[mPlayerId]->toggleGroupable();
            return;
        }

        // Direction
        sf::Vector2f direction = sf::Vector2f(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(mKeys.up)) {
            direction += sf::Vector2f(0.f, -1.f);
        }
        if (sf::Keyboard::isKeyPressed(mKeys.down)) {
            direction += sf::Vector2f(0.f, 1.f);
        }
        if (sf::Keyboard::isKeyPressed(mKeys.left)) {
            direction += sf::Vector2f(-1.f, 0.f);
        }
        if (sf::Keyboard::isKeyPressed(mKeys.right)) {
            direction += sf::Vector2f(1.f, 0.f);
        }
        direction = normalize(direction);
        mPlayers[mPlayerId]->setDirection(direction);
    }
}
