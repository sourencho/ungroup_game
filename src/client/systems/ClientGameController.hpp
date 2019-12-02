#ifndef ClientGameController_hpp
#define ClientGameController_hpp

#include <memory>
#include <queue>
#include <stdio.h>
#include <unordered_map>
#include <vector>

#include "../../common/resources/ResourceStore.hpp"
#include "../../common/systems/GameController.hpp"
#include "../rendering/Animation.hpp"
#include "../rendering/AnimationController.hpp"
#include "../rendering/GUIController.hpp"
#include "../systems/InputController.hpp"
#include "NetworkingClient.hpp"

class ClientGameController : public GameController {

  public:
    explicit ClientGameController(InputDef::InputKeys keys, sf::RenderWindow& window,
                                  sf::RenderTexture& buffer, sf::Vector2f buffer_scaling_factor,
                                  sf::Sprite& buffer_sprite);
    ~ClientGameController();

    void start();

  private:
    // Overrides
    void update(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) override;
    void preUpdate() override;
    void postUpdate() override;
    InputDef::PlayerInputs getPlayerInputs() override;
    void incrementTick() override;
    unsigned int getTick() override;
    void setTick(unsigned int tick) override;

    // Methods

    /**
     * Get the center coordinates of the player's view of the world.
     * Currently returns the center of the player's group.
     */
    sf::Vector2f getPlayerViewCenter();

    /**
     * Register the client via the NetworkingClient.
     * @returns: The client's player id.
     */
    void registerClient();

    void draw();

    /*
     * Apply the game state update from the server (rewind, because it is likely the state of an old
     * tick) and then interpolate up to the current tick by applying local inputs for each tick
     * (replay).
     */
    void rewindAndReplay();

    /**
     * Send client input to server
     */
    void sendInputs(std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> inputs);

    /**
     * Save local input for replay
     */
    void saveInputs(std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> inputs);

    // Variables
    uint32_t m_playerId;

    std::unordered_map<unsigned int, InputDef::ClientInputAndTick>
        m_tickToInput; // Cache of past inputs

    std::unique_ptr<NetworkingClient> m_networkingClient;
    std::unique_ptr<AnimationController> m_animationController;
    std::unique_ptr<InputController> m_inputController;
    std::unique_ptr<GUIController> m_guiController;

    sf::RenderWindow& m_window;
    sf::RenderTexture& m_buffer;
    sf::View m_playerView;
    sf::View m_windowView;
    sf::Vector2f m_bufferScalingFactor;
    sf::Sprite& m_bufferSprite;
};

#endif /* ClientGameController_hpp */
