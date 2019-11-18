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
    std::shared_ptr<PlayerInputs> collectInputs() override;
    void incrementTick() override;
    unsigned int getTick() override;
    void setTick(unsigned int tick) override;

    // Methods
    void addEventListeners();
    void handleEvents();
    void fetchPlayerId();
    void setClientUpdates();
    void rewindAndReplay();
    void handleCollisionEvent(std::shared_ptr<Event> event);
    void createCollisionAnimation(const sf::Vector2f& collision_position);
    PlayerInputs& getClientInputs(ClientReliableUpdate cru, ClientUnreliableUpdate cuu);

    // Variables

    ClientInputKeys m_clientInputKeys;

    bool m_playerIdAvailable;
    uint32_t m_playerId;

    ClientReliableUpdate m_clientReliableUpdate;
    ClientUnreliableUpdate m_clientUnreliableUpdate;

    PlayerInputs m_playerInputs;                                        // Cache of current input
    std::unordered_map<unsigned int, ClientInputAndTick> m_tickToInput; // Cache of past inputs

    std::unique_ptr<NetworkingClient> m_networkingClient;
    std::unique_ptr<AnimationController> m_animationController;

    sf::RenderWindow& m_window;
};

#endif /* ClientGameController_hpp */
