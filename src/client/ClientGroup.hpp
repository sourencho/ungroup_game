#ifndef ClientGroup_hpp
#define ClientGroup_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "../common/Circle.hpp"
#include "../common/GameObject.hpp"

class ClientGroup : public GameObject {

    public:
        ClientGroup(sf::Vector2f position);
        ~ClientGroup();

        Circle* getCircle();

    private:
        Circle* mCircle;
};

#endif /* ClientGroup_hpp */
