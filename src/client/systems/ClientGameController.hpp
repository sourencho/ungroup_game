#ifndef ClientGameController_hpp
#define ClientGameController_hpp

#include <memory>
#include <queue>
#include <stdio.h>
#include <unordered_map>
#include <vector>

#include "../../common/events/Event.hpp"
#include "../../common/resources/ResourceStore.hpp"
#include "../../common/systems/GameController.hpp"
#include "../rendering/Animation.hpp"
#include "../rendering/AnimationController.hpp"
#include "../systems/InputController.hpp"
#include "NetworkingClient.hpp"

class ClientGameController : public GameController {

  public:
    explicit ClientGameController(ClientInputKeys keys, sf::RenderWindow& window);
    ~ClientGameController();

    void updateView(sf::RenderWindow& window, sf::Vector2f buffer_scaling_factor);
    void draw(sf::RenderTexture& buffer);

  private:
    // Overrides
    void update(std::shared_ptr<PlayerInputs> pi, sf::Int32 delta_ms) override;
    void preUpdate() override;
    void postUpdate() override;
    std::shared_ptr<PlayerInputs> getPlayerInputs() override;
    void incrementTick() override;
    unsigned int getTick() override;
    void setTick(unsigned int tick) override;

    // Methods
    void addEventListeners();
    void fetchPlayerId();
    void rewindAndReplay();
    void handleCollisionEvent(std::shared_ptr<Event> event);
    void createCollisionAnimation(const sf::Vector2f& collision_position);
    void sendClientInputs(std::pair<ClientReliableUpdate, ClientUnreliableUpdate> client_inputs);
    void saveClientInputs(std::pair<ClientReliableUpdate, ClientUnreliableUpdate> client_inputs);

    // Variables

    bool m_playerIdAvailable;
    uint32_t m_playerId;

    std::unordered_map<unsigned int, ClientInputAndTick> m_tickToInput; // Cache of past inputs

    std::unique_ptr<NetworkingClient> m_networkingClient;
    std::unique_ptr<AnimationController> m_animationController;
    std::unique_ptr<InputController> m_inputController;

    sf::RenderWindow& m_window;
};

#endif /* ClientGameController_hpp */
