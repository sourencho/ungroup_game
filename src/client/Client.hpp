#ifndef Client_hpp
#define Client_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "NetworkingClient.hpp"
#include "GroupCircle.hpp"
#include "../common/util.hpp"
#include "../common/game_def.hpp"

class Client {

    public:
        Client(int max_player_count, sf::Keyboard::Key keys[4]);
        ~Client();

        void draw(sf::RenderTarget& target);
        void update();
        void handleEvents(sf::Event& event);

        // Setters
        void setId(sf::Uint32 id);

        // Getters
        sf::Uint32 getId() const;
        sf::Vector2f getDirection() const;

    private:
        sf::Uint32 initNetworking();
        std::vector<GroupCircle*> mGroupCircles;
        sf::Vector2f mDirection;
        keys mKeys;
        sf::Uint32 mId;

        NetworkingClient* mNetworkingClient;
};

#endif /* Client_hpp */
