#ifndef Group_hpp
#define Group_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "Player.hpp"
#include "../common/GroupShape.hpp"

class Group: public GroupShape {

    public:
        Group(int id, sf::Vector2f position);
        ~Group();
    
        void update();
        void draw(sf::RenderTarget& target);
    
        void addMember(Player* player);
    
        // Getters
        int getId() const;

    private:
        int mId;
        float mSize;
        std::vector<Player*> mMembers;
        bool shouldDeactivate();
};

#endif /* Group_hpp */
