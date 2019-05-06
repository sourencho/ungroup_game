#ifndef Group_hpp
#define Group_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "Player.hpp"

class Group{

    public:
        Group(float size, sf::Vector2f position, sf::Color color);
        ~Group();
    
        void update();
        void draw(sf::RenderTarget& target);
        void handleEvents(sf::Event& event);
        void addMemeber(Player* player);
    
    private:
        sf::CircleShape mCircle;
        sf::Vector2f mPosition;
        float mVelocity;
        std::vector<Player*> mMembers;
};

#endif /* Group_hpp */
