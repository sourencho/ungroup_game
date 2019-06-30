#ifndef Client_hpp
#define Client_hpp

#include <stdio.h>
#include <vector>
#include <unordered_map>

#include "../common/util.hpp"
#include "../common/game_def.hpp"
#include "NetworkingClient.hpp"
#include "ClientGroup.hpp"
#include <SFML/Graphics.hpp>

class Client {
 public:
     Client(int max_player_count, sf::Keyboard::Key keys[4]);
     ~Client();
     Client(const Client& temp_obj) = delete;
     Client& operator=(const Client& temp_obj) = delete;

     // Methods
     void draw(sf::RenderTarget& target, sf::Shader* shader);
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
     int createClientGroup();
     void refreshClientGroups(std::vector<int> client_ids);
     void updateClientGroups(std::vector<ClientGroupUpdate> client_group_updates);

     // Variables
     std::vector<ClientGroup*> mClientGroups;
     sf::Vector2f mDirection;
     keys mKeys;
     sf::Uint32 mId;
     NetworkingClient* mNetworkingClient;
     std::unordered_map<sf::Uint32, int> mClientToClientGroup;
     size_t mNextClientGroupId = 0;
};

#endif /* Client_hpp */
