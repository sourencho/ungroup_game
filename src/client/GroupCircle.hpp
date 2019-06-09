#ifndef GroupCircle_hpp
#define GroupCircle_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "../common/Circle.hpp"
#include "../common/GameObject.hpp"

class GroupCircle : public GameObject {

    public:
        GroupCircle(int id, sf::Vector2f position);
        ~GroupCircle();

        Circle* getCircle();

    private:
        Circle* mCircle;
};

#endif /* GroupCircle_hpp */
