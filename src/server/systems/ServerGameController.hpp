#ifndef ServerGameController_hpp
#define ServerGameController_hpp

#include <memory>
#include <stdio.h>
#include <unordered_map>
#include <vector>

#include "../../common/events/Event.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/systems/GameController.hpp"
#include "../../common/systems/LevelController.hpp"
#include "../rendering/TerminalRenderingController.hpp"
#include "../../common/events/CollisionEvent.hpp"

#include "NetworkingServer.hpp"

class ServerGameController : public GameController {
  public:
    explicit ServerGameController(LevelKey level_key = LevelKey::mine_ring,
                                  uint32_t tcp_port = GAME_SETTINGS.SERVER_TCP_PORT);
    ~ServerGameController();

    void start() override;
    void handleCollisionEvent(std::shared_ptr<Event> event);

  private:
    // Overrides
    void draw();
    void update(const InputDef::PlayerInputs& pi, sf::Int32 delta_ms) override;
    void preUpdate() override;
    void postUpdate(sf::Int32 update_time) override;
    InputDef::PlayerInputs getPlayerInputs() override;
    void incrementTick() override;

    // Methods
    void setNetworkState();
    void addEventListeners();

    // Variables
    std::unique_ptr<NetworkingServer> m_networkingServer;
    TerminalRenderingController m_terminalRenderingController;
    GameStateEvent m_gameStateEvent;
};

#endif /* ServerGameController_hpp */
