#ifndef Client_hpp
#define Client_hpp

#include <stdio.h>
#include <vector>
#include <unordered_map>

#include "../common/util.hpp"
#include "../common/game_def.hpp"
#include "../common/PhysicsController.hpp"
#include "../common/Player.hpp"
#include "NetworkingClient.hpp"
#include "ClientGroup.hpp"
#include "ClientMine.hpp"
#include <SFML/Graphics.hpp>

class Client {
 public:
     Client(int max_player_count, int max_mine_count, sf::Keyboard::Key keys[4]);
     ~Client();
     Client(const Client& temp_obj) = delete;
     Client& operator=(const Client& temp_obj) = delete;

     // Methods
     void draw(sf::RenderTarget& target, sf::Shader* shader, bool use_shader);
     void update();
     void handleEvents(sf::Event& event);

     // Setters
     void setId(sf::Uint32 id);

     // Getters
     sf::Uint32 getId() const;
     sf::Vector2f getDirection() const;

 private:
     // Methods
     sf::Uint32 initNetworking();

     // Variables
     std::vector<ClientGroup*> mClientGroups;
     std::vector<ClientMine*> mClientMines;
     std::vector<Player*> mPlayers;

     sf::Vector2f mDirection;
     Keys mKeys;
     bool mGroupable = false;
     sf::Uint32 mId;

     std::shared_ptr<PhysicsController> mPhysicsController;
     std::unique_ptr<NetworkingClient> mNetworkingClient;
};

#endif /* Client_hpp */
