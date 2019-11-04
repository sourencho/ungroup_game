#include <thread>

#include "../../common/events/CollisionEvent.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/physics/VectorUtil.hpp"
#include "../../common/util/game_state.hpp"
#include "ClientGameController.hpp"
#include <SFML/Graphics.hpp>

ClientGameController::ClientGameController(size_t max_player_count, size_t max_mine_count,
                                           Keys keys)
    : GameController(max_player_count, max_mine_count), mNetworkingClient(new NetworkingClient()),
      mAnimationController(new AnimationController()), mKeys(keys) {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_COLLISION,
        std::bind(&ClientGameController::clientCollisionEvent, this, std::placeholders::_1));
}

ClientGameController::~ClientGameController() {}

ClientInputs ClientGameController::collectInputs() {
    return getClientInputs(mClientReliableUpdate, mClientUnreliableUpdate);
}

void ClientGameController::setNetworkState() {
    // noop
}

void ClientGameController::incrementTick() { mNetworkingClient->incrementTick(); }

void ClientGameController::updateView(sf::RenderWindow& window,
                                      sf::Vector2f buffer_scaling_factor) {
    // Update view to match player's group's position
    sf::Vector2f group_position =
        mGameObjectStore->getGroup(mGroupController->getGroupId(mPlayerId))->getCenter();
    sf::View view = window.getView();
    sf::Vector2f group_view_coordinates = {group_position.x * buffer_scaling_factor.x,
                                           group_position.y * buffer_scaling_factor.y};
    view.setCenter(group_view_coordinates);
    window.setView(view);
}

void ClientGameController::draw(sf::RenderTarget& target) {
    mGroupController->draw(target);
    mMineController->draw(target);
    mAnimationController->draw(target);
}

void ClientGameController::handleEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (sf::Keyboard::isKeyPressed(mKeys.group)) {
            mClientReliableUpdate.joinable ^= true;
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
        direction = VectorUtil::normalize(direction);
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
    }
    GameController::update();
}

void ClientGameController::step(const ClientInputs& cis, sf::Int32 delta_ms) {
    computeGameState(cis, delta_ms);
    mAnimationController->step(delta_ms);
}

void ClientGameController::rewindAndReplay() {
    // Rewind
    GameState game_state = mNetworkingClient->getGameState();
    applyGameState(game_state);

    // Replay
    int tick_delta = getTick() - game_state.tick;
    if (tick_delta <= 0) {
        return;
    } // If the client is behind the server we don't need to replay

    // Loop through ticks that need to be replayed and apply client input from cache if present
    for (int i = 0; i < tick_delta; ++i) {
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
            mPlayerController->setPlayerClient(mPlayerId, mNetworkingClient->getClientId());
        }
    }
}

void ClientGameController::setClientUpdates() {
    // Set input to send to server
    mNetworkingClient->setClientUnreliableUpdate(mClientUnreliableUpdate);
    mNetworkingClient->setClientReliableUpdate(mClientReliableUpdate);

    // Save input and state for replay
    mTickToInput[mNetworkingClient->getTick()] = (ClientInputAndTick){
        mClientUnreliableUpdate, mClientReliableUpdate, mNetworkingClient->getTick()};
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

void ClientGameController::clientCollisionEvent(std::shared_ptr<Event> event) {
    switch (event->getType()) {
        case EventType::EVENT_TYPE_COLLISION: {
            std::shared_ptr<CollisionEvent> collision_event =
                std::dynamic_pointer_cast<CollisionEvent>(event);
            sf::Vector2f position = collision_event->getCollision().position;

            // Create collision animation
            auto collision = std::unique_ptr<AnimatedSprite>(new AnimatedSprite(
                *mResourceStore->getTexture("collision"), {6, 1}, 240, position, {2.f, 2.f}));
            mAnimationController->add(std::move(collision));
            break;
        }
        default: {
            std::cout << "Unexpected event type." << std::endl;
            break;
        }
    }
}

unsigned int ClientGameController::getTick() { return mNetworkingClient->getTick(); }

void ClientGameController::setTick(unsigned int tick) { mNetworkingClient->setTick(tick); }
