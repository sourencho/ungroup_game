#ifndef Group_hpp
#define Group_hpp

#include <stdio.h>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "../common/Circle.hpp"
#include "../common/CircleGameObject.hpp"

class Group : public CircleGameObject {
 public:
     Group(int id, sf::Vector2f position, sf::Color color, std::shared_ptr<PhysicsController> pc);
     ~Group();
     Group(const Group& temp_obj) = delete;  // TODO: define this
     Group& operator=(const Group& temp_obj) = delete;  // TODO: define this

     void update();
     bool getGroupable();
     void setGroupable(bool);
     void addMember(std::shared_ptr<Player> player);

 private:
     std::vector<std::shared_ptr<Player>> mMembers;
     void refresh();

     bool mGroupable = false;
};

#endif /* Group_hpp */
