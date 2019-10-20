#ifndef ClientGameController_hpp
#define ClientGameController_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>

#include "../rendering/Animation.hpp"
#include "../rendering/AnimationController.hpp"
#include "../../common/systems/GameController.hpp"
#include "NetworkingClient.hpp"


class ClientGameController : public GameController {

 public:
    explicit ClientGameController(size_t max_player_count, size_t max_mine_count, Keys keys);
    ~ClientGameController();

    void update() override;
    void draw(sf::RenderTarget& target, sf::Shader* shader, bool use_shader);
    void handleEvents(sf::Event& event);
 private:
    ClientInputs collectInputs() override;
    void setNetworkState() override;
    void incrementTick() override;
    unsigned int getTick() override;
    void setTick(unsigned int tick) override;
    void step(const ClientInputs& cis, sf::Int32 delta_ms) override;

    void fetchPlayerId();
    void setClientUpdates();
    void rewindAndReplay();

    ClientInputs& getClientInputs(ClientReliableUpdate cru, ClientUnreliableUpdate cuu);

    Keys mKeys;
    int mPlayerId = -1;

    ClientReliableUpdate mClientReliableUpdate;
    ClientUnreliableUpdate mClientUnreliableUpdate;

    ClientInputs mClientInputs; // Cache of current client input
    std::unordered_map<unsigned int, ClientInputAndTick> mTickToInput; // Cache of player inputs

    std::unique_ptr<NetworkingClient> mNetworkingClient;
    std::unique_ptr<AnimationController> mAnimationController;
};

#endif /* ClientGameController_hpp */
