#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include "Client.hpp"

Client::Client(int max_player_count, int max_mine_count, sf::Keyboard::Key keys[5]):
  mDirection(1.0, 1.0), mPhysicsController(new PhysicsController()),
  mNetworkingClient(new NetworkingClient()) {
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
    GameState game_state =  mNetworkingClient->getGameState();

    for (auto gu : game_state.group_updates) {
        mClientGroups[gu.group_id]->applyUpdate(gu);
    }

    for (auto mu : game_state.mine_updates) {
        mClientMines[mu.mine_id]->applyUpdate(mu);
    }

    // Set direction
    mNetworkingClient->setDirection(mDirection);
    mNetworkingClient->setGroupable(mGroupable);
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
