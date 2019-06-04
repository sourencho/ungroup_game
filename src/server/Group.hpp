#ifndef Group_hpp
#define Group_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "Player.hpp"
#include "../common/GameObject.hpp"

class Group: public GameObject {

    public:
        Group(int id, sf::Vector2f position);
        ~Group();
    
        void update();
        void draw(sf::RenderTarget& target);
    
        void addMember(Player* player);
    
        // Getters
        sf::Vector2f getPosition() const;
        float getSize() const;
        int getId() const;

        // Setters
        void move(const sf::Vector2f& offset);
    
    private:
        int mId;
        float mSize;
        sf::Vector2f mPosition;
        std::vector<Player*> mMembers;
        bool shouldDeactivate();
};

#endif /* Group_hpp */
