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
    explicit ClientGameController(size_t max_player_count, size_t max_mine_count, Keys keys);
    ~ClientGameController();

    void update() override;
    void updateView(sf::RenderWindow& window, sf::Vector2f buffer_scaling_factor);
    void draw(sf::RenderTarget& target);
    void handleEvents(sf::Event& event);

  private:
    void addEventListeners();
    ClientInputs collectInputs() override;
    void setNetworkState() override;
    void incrementTick() override;
    unsigned int getTick() override;
    void setTick(unsigned int tick) override;
    void step(const ClientInputs& cis, sf::Int32 delta_ms) override;

    void fetchPlayerId();
    void setClientUpdates();
    void rewindAndReplay();
    void handleCollisionEvent(std::shared_ptr<Event> event);
    void createCollisionAnimation(const sf::Vector2f& collision_position);

    ClientInputs& getClientInputs(ClientReliableUpdate cru, ClientUnreliableUpdate cuu);

    Keys m_Keys;
    int m_PlayerId = -1;

    ClientReliableUpdate m_ClientReliableUpdate;
    ClientUnreliableUpdate m_ClientUnreliableUpdate;

    ClientInputs m_ClientInputs;                                        // Cache of current input
    std::unordered_map<unsigned int, ClientInputAndTick> m_TickToInput; // Cache of past inputs

    std::unique_ptr<NetworkingClient> m_NetworkingClient;
    std::unique_ptr<AnimationController> m_AnimationController;
};

#endif /* ClientGameController_hpp */
