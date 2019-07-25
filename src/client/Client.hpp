#ifndef Client_hpp
#define Client_hpp

#include <stdio.h>
#include <vector>
#include <unordered_map>

#include "../common/util.hpp"
#include "../common/game_def.hpp"
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
     int createClientGroup();
     int createClientMine();
     void refreshClientGroups(std::vector<int> group_ids);
     void updateClientGroups(std::vector<GroupUpdate> group_updates);
     void refreshClientMines(std::vector<int> group_ids);
     void updateClientMines(std::vector<MineUpdate> mine_updates);

     // Variables
     std::vector<ClientGroup*> mClientGroups;
     std::vector<ClientMine*> mClientMines;
     sf::Vector2f mDirection;
     keys mKeys;
     sf::Uint32 mId;
     NetworkingClient* mNetworkingClient;
     std::unordered_map<sf::Uint32, int> mGroupIdToClientGroup;
     std::unordered_map<sf::Uint32, int> mMineIdToClientMine;
     size_t mNextClientGroupId = 0;
     size_t mNextClientMineId = 0;
};

#endif /* Client_hpp */
