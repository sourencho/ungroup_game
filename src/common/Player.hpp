#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>

#include <SFML/Graphics.hpp>
#include "../common/GameObject.hpp"

struct PlayerUpdate {
    int id;
    sf::Vector2f direction;
    bool groupable;
};

class Player: public GameObject {
 public:
     explicit Player(int id);
     ~Player();

     void setDirection(sf::Vector2f direction);
     void setGroupable(bool groupable);
     sf::Vector2f getDirection() const;
     bool getGroupable() const;

     PlayerUpdate get_player_update() const;
     void apply_player_update(const PlayerUpdate& pu);

 private:
     sf::Vector2f mDirection;
     bool mGroupable;
};


#endif /* Player_hpp */
