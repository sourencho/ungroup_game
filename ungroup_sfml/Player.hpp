//
//  Player.hpp
//  ungroup_sfml
//
//  Created by Souren Papazian on 5/5/19.
//  Copyright © 2019 Souren Papazian. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>

class Player {
    
public:
    Player(sf::Keyboard::Key keys[4]);
    ~Player();
    
    void update();
    void handleEvents(sf::Event& event);
    sf::Vector2f getDirection() const;
    
private:
    sf::Vector2f mDirection;
    sf::Keyboard::Key mUp;
    sf::Keyboard::Key mDown;
    sf::Keyboard::Key mLeft;
    sf::Keyboard::Key mRigth;
};


#endif /* Player_hpp */
