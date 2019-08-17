#ifndef ClientGroup_hpp
#define ClientGroup_hpp

#include <stdio.h>

#include <SFML/Graphics.hpp>
#include "../common/Circle.hpp"
#include "../common/CircleGameObject.hpp"

class ClientGroup : public CircleGameObject {
 public:
     explicit ClientGroup(sf::Vector2f position, sf::Color color);
     ~ClientGroup();
     void setGroupable(bool groupable);
     bool getGroupable();
     ClientGroup(const ClientGroup& temp_obj) = delete;  // TODO(sourenp): define this
     ClientGroup& operator=(const ClientGroup& temp_obj) = delete;  // TODO(sourenp): define this

 private:
     bool mGroupable = false;
};

#endif /* ClientGroup_hpp */
