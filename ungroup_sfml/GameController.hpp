#ifndef GameController_hpp
#define GameController_hpp

#include <stdio.h>
#include "Player.hpp"
#include "Group.hpp"

class GameController {

    public:
        GameController();
        ~GameController();
        void update();
        void createPlayer(sf::Keyboard::Key keys[4]);
        const std::vector<Group*> &getGroups();
        void draw(sf::RenderTarget& target);
        void handleEvents(sf::Event& event);

    private:
        std::vector<Player*> mPlayers;
        std::vector<Group*> mGroups;
};
    
#endif /* GameController_hpp */
