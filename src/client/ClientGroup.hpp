#ifndef ClientGroup_hpp
#define ClientGroup_hpp

#include <stdio.h>
#include <memory>

#include <SFML/Graphics.hpp>

#include "../common/Circle.hpp"
#include "../common/CircleGameObject.hpp"
#include "../common/game_def.hpp"
#include "../common/Group.hpp"


class ClientGroup : public CircleGameObject {
 public:
     explicit ClientGroup(sf::Vector2f position, sf::Color color,
     std::shared_ptr<PhysicsController> pc);
     ~ClientGroup();
     void setGroupable(bool groupable);
     bool getGroupable();
     ClientGroup(const ClientGroup& temp_obj) = delete;  // TODO(sourenp): define this
     ClientGroup& operator=(const ClientGroup& temp_obj) = delete;  // TODO(sourenp): define this

     void applyUpdate(GroupUpdate gu);

 private:
     bool mGroupable = false;
};

#endif /* ClientGroup_hpp */
