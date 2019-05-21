#ifndef Player_hpp
#define Player_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "GameObject.hpp"

class Player: public GameObject {

    public:
        Player();
        ~Player();

        void setDirection(sf::Vector2f direction);
        sf::Vector2f getDirection() const;

    private:
        sf::Vector2f mDirection;

};


#endif /* Player_hpp */
