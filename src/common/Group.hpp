#ifndef Group_hpp
#define Group_hpp

#include <stdio.h>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "../common/Player.hpp"
#include "../common/game_def.hpp"
#include "../common/Circle.hpp"
#include "../common/CircleGameObject.hpp"


struct GroupUpdate {
    sf::Uint32 group_id;
    bool is_active;
    float x_pos;
    float y_pos;
    float radius;
    bool groupable;
};

sf::Packet& operator <<(sf::Packet& packet, const GroupUpdate& group_update);
sf::Packet& operator >>(sf::Packet& packet, GroupUpdate& group_update);

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

     GroupUpdate getUpdate();

 private:
     std::vector<std::shared_ptr<Player>> mMembers;
     void refresh();

     bool mGroupable = false;
};

#endif /* Group_hpp */
