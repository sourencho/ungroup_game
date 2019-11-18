#ifndef ServerGameController_hpp
#define ServerGameController_hpp

#include <memory>
#include <stdio.h>
#include <unordered_map>
#include <vector>

#include "../../common/events/Event.hpp"
#include "../../common/systems/GameController.hpp"
#include "NetworkingServer.hpp"

class ServerGameController : public GameController {
  public:
    explicit ServerGameController();
    ~ServerGameController();

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
    void setNetworkState();

    // Variables
    std::unique_ptr<NetworkingServer> m_networkingServer;
};

#endif /* ServerGameController_hpp */
