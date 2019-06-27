#ifndef ClientGroup_hpp
#define ClientGroup_hpp

#include <stdio.h>

#include <SFML/Graphics.hpp>
#include "../common/Circle.hpp"
#include "../common/GameObject.hpp"

class ClientGroup : public GameObject {
 public:
     explicit ClientGroup(sf::Vector2f position);
     ~ClientGroup();
     ClientGroup(const ClientGroup& temp_obj) = delete;  // TODO(sourenp): define this
     ClientGroup& operator=(const ClientGroup& temp_obj) = delete;  // TODO(sourenp): define this

     Circle* getCircle();

 private:
     Circle* mCircle;
};

#endif /* ClientGroup_hpp */
