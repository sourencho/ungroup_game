#ifndef Client_hpp
#define Client_hpp

#include <SFML/Graphics.hpp>
#include "Group.hpp"
#include "GroupShape.hpp"
#include <stdio.h>

class Client {

    public:
        Client(int player_count);
        ~Client();

        void draw(sf::RenderTarget& target);
        void update(std::vector<Group*> groups);

    private:
        std::vector<GroupShape*> mGroupShapes;

};

#endif /* Client_hpp */
