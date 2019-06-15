#ifndef ClientGroup_hpp
#define ClientGroup_hpp

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "../common/Circle.hpp"
#include "../common/GameObject.hpp"

class ClientGroup : public GameObject {

    public:
        explicit ClientGroup(sf::Vector2f position);
        ~ClientGroup();
        ClientGroup(const ClientGroup& temp_obj) = delete;  // TODO: define this
        ClientGroup& operator=(const ClientGroup& temp_obj) = delete;  // TODO: define this

        Circle* getCircle();

    private:
        Circle* mCircle;
};

#endif /* ClientGroup_hpp */
