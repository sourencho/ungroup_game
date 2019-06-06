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
        std::vector<Group*> getActiveGroups();

    private:
        void createPlayer(int new_player_id);
        void updateState();

        std::vector<Player*> mPlayers;
        std::vector<Group*> mGroups;
        NetworkingServer* mNetworkingServer;
};

#endif /* GameController_hpp */
