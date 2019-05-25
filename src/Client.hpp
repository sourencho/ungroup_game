#ifndef Client_hpp
#define Client_hpp

#include <SFML/Graphics.hpp>
#include "Group.hpp"
#include "GroupShape.hpp"
#include "game_def.hpp"
#include <stdio.h>
#include "networking_client.hpp"

class Client {

    public:
        Client(int max_player_count, sf::Keyboard::Key keys[4]);
        ~Client();

        void draw(sf::RenderTarget& target);
        void update(std::vector<Group*> active_groups);
        void handleEvents(sf::Event& event);

        // Setters
        void setId(sf::Uint32 id);

        // Getters
        sf::Uint32 getId() const;
        sf::Vector2f getDirection() const;

    private:
        sf::Uint32 initNetworking();
        std::vector<GroupShape*> mGroupShapes;
        sf::Vector2f mDirection;
        keys mKeys;
        sf::Uint32 mId;

        sf::TcpSocket* mApiClient;
        sf::UdpSocket* mRealtimeClient;
};

#endif /* Client_hpp */
