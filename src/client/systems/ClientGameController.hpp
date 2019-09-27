#ifndef ClientGameController_hpp
#define ClientGameController_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include <unordered_map>

#include "../../common/systems/GameController.hpp"
#include "NetworkingClient.hpp"


class ClientGameController : public GameController {
 public:
   explicit ClientGameController(size_t max_player_count, size_t max_mine_count,
     sf::Keyboard::Key keys[5]);
   ~ClientGameController();

   void update() override;
   void draw(sf::RenderTarget& target, sf::Shader* shader, bool use_shader);
   void handleEvents(sf::Event& event);
 private:
   ClientInputs collectInputs() override;
   void setNetworkState() override;
   void incrementTick() override;

   void fetchPlayerId();
   void setClientUpdates();

   Keys mKeys;
   int mPlayerId = -1;

   ClientReliableUpdate mClientReliableUpdate;
   ClientUnreliableUpdate mClientUnreliableUpdate;

   std::unique_ptr<NetworkingClient> mNetworkingClient;
};

#endif /* ClientGameController_hpp */
