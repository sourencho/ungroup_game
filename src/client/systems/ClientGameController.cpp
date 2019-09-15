#include <thread>

#include "ClientGameController.hpp"
#include <SFML/Graphics.hpp>
#include "../../common/util/util.hpp"


int CLIENT_STEP_SLEEP = 60;

ClientGameController::ClientGameController(size_t max_player_count, size_t max_mine_count,
  sf::Keyboard::Key keys[5]): GameController(max_player_count, max_mine_count),
  mNetworkingClient(new NetworkingClient()) {
    mKeys.up = keys[0];
    mKeys.down = keys[1];
    mKeys.right = keys[2];
    mKeys.left = keys[3];
    mKeys.group = keys[4];
}

ClientGameController::~ClientGameController() {}

ClientInputs ClientGameController::collectInputs() {
    // TODO(souren): Optimize by reusing vectors instead of allocating new ones every time
    std::vector<ClientIdAndUnreliableUpdate> client_id_and_unreliable_updates;
    std::vector<ClientIdAndReliableUpdate> client_id_and_reliable_updates;
    int client_id = static_cast<int>(mNetworkingClient->getClientId());

    if (mPlayerId != -1) {
        ClientIdAndUnreliableUpdate client_id_and_unreliable_update = {client_id,
            mClientUnreliableUpdate};
        client_id_and_unreliable_updates.push_back(client_id_and_unreliable_update);

        ClientIdAndReliableUpdate client_id_and_reliable_update = {client_id,
            mClientReliableUpdate};
        client_id_and_reliable_updates.push_back(client_id_and_reliable_update);
    }

    ClientInputs client_inputs = {client_id_and_unreliable_updates, client_id_and_reliable_updates};
    return client_inputs;
}

void ClientGameController::setNetworkState() {
    // noop
}

void ClientGameController::incrementTick() {
    // noop
}

void ClientGameController::draw(sf::RenderTarget& target, sf::Shader* shader, bool use_shader) {
    for (auto group : mLevelController->getGroups()) {
        if (group->isActive()) {
            bool groupable = group->getGroupable();
            std::shared_ptr<Circle> circle = group->getCircle();
            if (groupable) {
                circle->changeColor(sf::Color(255, 0, 0));
            } else {
                circle->setColor();
            }
            circle->draw(target, shader, use_shader);
        }
    }

    for (auto mine : mLevelController->getMines()) {
        if (mine->isActive()) {
            mine->getCircle()->draw(target, shader, use_shader);
        }
    }
}

void ClientGameController::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (sf::Keyboard::isKeyPressed(mKeys.group)) {
            mClientReliableUpdate.groupable ^= true;
            return;
        }

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
        mClientUnreliableUpdate.direction = direction;
    }
}

void ClientGameController::step() {
    if (mPlayerId == -1) {
        mPlayerId = mNetworkingClient->getPlayerId();
        if (mPlayerId != -1) {
            mClientToPlayer[mNetworkingClient->getClientId()] = mPlayerId;
        }
    }

    if (mNetworkingClient->getGameStateIsFresh()) {
        GameState game_state =  mNetworkingClient->getGameState();
        applyGameState(game_state);
    } else {  // interpolate
        GameController::step();
    }

    mNetworkingClient->setClientReliableUpdate(mClientReliableUpdate);
    mNetworkingClient->setClientUnreliableUpdate(mClientUnreliableUpdate);
    std::this_thread::sleep_for(std::chrono::milliseconds(CLIENT_STEP_SLEEP));
}
