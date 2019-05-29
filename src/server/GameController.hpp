#ifndef GameController_hpp
#define GameController_hpp

#include <stdio.h>
#include "Player.hpp"
#include "Group.hpp"
#include "CollisionDetector.hpp"
#include "NetworkingServer.hpp"


class GameController {

    public:
        GameController(int max_player_count);
        ~GameController();
        void update(int client_id, sf::Vector2f client_direction);
        size_t createPlayer();
        std::vector<Group*> getActiveGroups();
        void draw(sf::RenderTarget& target);

    private:
        std::vector<Player*> mPlayers;
        std::vector<Group*> mGroups;
        CollisionDetector* mCollisionDetector;
        int mNextGroupId = 0;
        int mNextPlayerId = 0;
        NetworkingServer* mNetworkingServer;
};

#endif /* GameController_hpp */
