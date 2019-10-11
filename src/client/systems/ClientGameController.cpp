#include <thread>

#include "ClientGameController.hpp"
#include <SFML/Graphics.hpp>
#include "../../common/util/util.hpp"
#include "../../common/util/game_state.hpp"


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
    return getClientInputs(mClientReliableUpdate, mClientUnreliableUpdate);
}

void ClientGameController::setNetworkState() {
    // noop
}

void ClientGameController::incrementTick() {
    mNetworkingClient->incrementTick();
}

void ClientGameController::draw(sf::RenderTarget& target, sf::Shader* shader, bool use_shader) {
    for (auto group : mGameObjectStore->getGroups()) {
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

    for (auto mine : mGameObjectStore->getMines()) {
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

/**
 * Fetch player id from server and set the client updates to send to the server then update game
 * state. This is either done via rewind and replay when we have a game state update from the server
 * or via local interpolation.
 * Rewind and replay is applying the game state update from the server (rewind because it is likely
 * for an old tick) and then interpolating up to the current tick via interpolation (replay).
**/
void ClientGameController::update() {
    fetchPlayerId();
    setClientUpdates();

    if (mNetworkingClient->getGameStateIsFresh()) {
        rewindAndReplay();
        GameController::update();
    } else {
        GameController::update();  // interpolate
    }
}

void ClientGameController::rewindAndReplay() {
    // Rewind
    GameState game_state = mNetworkingClient->getGameState();
    applyGameState(game_state);

    // Replay
    int tick_delta = getTick() - game_state.tick;
    if (tick_delta <= 0) {return;}  // If the client is behind the server we don't need to replay

    // Loop through ticks that need to be replayed and apply client input from cache if present
    for (int i=0; i < tick_delta; ++i) {
        ClientInputAndTick client_input_and_tick;
        unsigned int replay_tick = game_state.tick + i;

        if (mTickToInput.count(replay_tick) > 0) {
            client_input_and_tick = mTickToInput[replay_tick];
            GameController::computeGameState(
                getClientInputs(client_input_and_tick.cru, client_input_and_tick.cuu),
                GameController::MIN_TIME_STEP);
        } else {
            // If we don't have input for this tick pass in empty ClientInputs
            GameController::computeGameState(ClientInputs(), GameController::MIN_TIME_STEP);
        }
    }
    mTickToInput.clear();
}

void ClientGameController::fetchPlayerId() {
    if (mPlayerId == -1) {
        mPlayerId = mNetworkingClient->getPlayerId();
        if (mPlayerId != -1) {
            mClientToPlayer[mNetworkingClient->getClientId()] = mPlayerId;
        }
    }
}

void ClientGameController::setClientUpdates() {
    // Set input to send to server
    mNetworkingClient->setClientUnreliableUpdate(mClientUnreliableUpdate);
    mNetworkingClient->setClientReliableUpdate(mClientReliableUpdate);

    // Save input and state for replay
    mTickToInput[mNetworkingClient->getTick()] = (ClientInputAndTick){mClientUnreliableUpdate,
        mClientReliableUpdate, mNetworkingClient->getTick()};
}

ClientInputs& ClientGameController::getClientInputs(ClientReliableUpdate cru,
  ClientUnreliableUpdate cuu) {
    int client_id = static_cast<int>(mNetworkingClient->getClientId());

    if (mClientInputs.client_id_and_reliable_updates.size() == 0) {
        mClientInputs.client_id_and_reliable_updates.resize(1);
    }
    mClientInputs.client_id_and_reliable_updates[0].client_id = client_id;
    mClientInputs.client_id_and_reliable_updates[0].client_reliable_update = cru;

    if (mClientInputs.client_id_and_unreliable_updates.size() == 0) {
        mClientInputs.client_id_and_unreliable_updates.resize(1);
    }
    mClientInputs.client_id_and_unreliable_updates[0].client_id = client_id;
    mClientInputs.client_id_and_unreliable_updates[0].client_unreliable_update = cuu;

    return mClientInputs;
}

unsigned int ClientGameController::getTick() {
    return mNetworkingClient->getTick();
}

void ClientGameController::setTick(unsigned int tick) {
    mNetworkingClient->setTick(tick);
}
