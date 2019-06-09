#ifndef GameController_hpp
#define GameController_hpp

#include <stdio.h>
#include "Player.hpp"
#include "Group.hpp"
#include "NetworkingServer.hpp"


class GameController {

    public:
        GameController(int max_player_count);
        ~GameController();
        void update();

    private:
        int createPlayer();
        void getClientInput();
        void updateClientPlayers(std::vector<int> client_ids);
        void updatePlayers(std::vector<client_direction_update> client_direction_updates);
        void updateGroups();
        void updateNetworkState();
        void incrementTick();
        std::vector<Group*> getActiveGroups();

        std::vector<Player*> mPlayers;
        std::vector<Group*> mGroups;
        NetworkingServer* mNetworkingServer;
        std::unordered_map<sf::Uint32, int> mClientToPlayer;
        int mNextPlayerId = 0;
};

#endif /* GameController_hpp */
