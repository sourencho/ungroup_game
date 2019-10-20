#ifndef ServerGameController_hpp
#define ServerGameController_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include <unordered_map>

#include "../../common/systems/GameController.hpp"
#include "NetworkingServer.hpp"
#include "../../common/events/Event.hpp"


class ServerGameController : public GameController {
 public:
    explicit ServerGameController(size_t max_player_count, size_t max_mine_count);
    ~ServerGameController();

    void clientConnected(std::shared_ptr<Event> event);
    void clientDisconnected(std::shared_ptr<Event> event);
 private:
    ClientInputs collectInputs() override;
    void setNetworkState() override;
    void incrementTick() override;
    unsigned int getTick() override;
    void setTick(unsigned int tick) override;
    void step(const ClientInputs& cis, sf::Int32 delta_ms) override;

    std::unique_ptr<NetworkingServer> mNetworkingServer;
};

#endif /* ServerGameController_hpp */
