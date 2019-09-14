#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "GameObject.hpp"


struct PlayerUpdate {
    sf::Uint32 player_id;
    sf::Vector2f direction;
    bool groupable;
};

sf::Packet& operator <<(sf::Packet& packet, const PlayerUpdate& player_update);
sf::Packet& operator >>(sf::Packet& packet, PlayerUpdate& player_update);

class Player: public GameObject {
 public:
     explicit Player(int id);
     ~Player();

     void setDirection(sf::Vector2f direction);
     void setGroupable(bool groupable);
     sf::Vector2f getDirection() const;
     bool getGroupable() const;

 private:
     sf::Vector2f mDirection;
     bool mGroupable = false;
};


#endif /* Player_hpp */
