#ifndef Group_hpp
#define Group_hpp

#include <memory>
#include <stdio.h>
#include <vector>

#include "../physics/PhysicsController.hpp"
#include "../resources/ResourceStore.hpp"
#include "Circle.hpp"
#include "CircleGameObject.hpp"
#include "Player.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

struct GroupUpdate {
    sf::Uint32 group_id;
    bool is_active;
    float x_pos;
    float y_pos;
    float radius;
    bool groupable;
    sf::Uint32 shader_key;
};

sf::Packet& operator<<(sf::Packet& packet, const GroupUpdate& group_update);
sf::Packet& operator>>(sf::Packet& packet, GroupUpdate& group_update);

class Group : public CircleGameObject {
  public:
    Group(uint32_t id, sf::Vector2f position, sf::Color color,
          std::shared_ptr<PhysicsController> pc, ResourceStore& rs);
    ~Group();
    Group(const Group& temp_obj) = delete;            // TODO: define this
    Group& operator=(const Group& temp_obj) = delete; // TODO: define this

    bool getGroupable();
    void setGroupable(bool groupable);

    GroupUpdate getUpdate();
    void applyUpdate(GroupUpdate gu);

  private:
    bool mGroupable = false;
};

#endif /* Group_hpp */
