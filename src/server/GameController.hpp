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
        client_inputs collectInputs();
        void computeGameState(
            std::vector<int> client_ids, std::vector<client_direction_update> client_direction_updates);
        void refreshPlayers(std::vector<int> client_ids);
        void updatePlayers(std::vector<client_direction_update> client_direction_updates);
        void refreshAndUpdateGroups();
        void setNetworkState();
        void incrementTick();
        std::vector<Group*> getActiveGroups();

        std::vector<Player*> mPlayers;
        std::vector<Group*> mGroups;
        NetworkingServer* mNetworkingServer;
        std::unordered_map<sf::Uint32, int> mClientToPlayer;
        size_t mNextPlayerId = 0;
};

#endif /* GameController_hpp */
