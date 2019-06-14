#ifndef Client_hpp
#define Client_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "NetworkingClient.hpp"
#include "ClientGroup.hpp"
#include "../common/util.hpp"
#include "../common/game_def.hpp"
#include <unordered_map>

class Client {

    public:
        Client(int max_player_count, sf::Keyboard::Key keys[4]);
        ~Client();

        // Methods
        void draw(sf::RenderTarget& target);
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
        void updateClientGroups(std::vector<group_circle_update> group_circle_updates);

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
