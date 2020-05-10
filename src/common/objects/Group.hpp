#ifndef Group_hpp
#define Group_hpp

#include <memory>
#include <stdio.h>
#include <vector>

#include <SFML/Network.hpp>

#include "../physics/PhysicsController.hpp"
#include "../systems/ResourceController.hpp"
#include "CircleGameObject.hpp"
#include "Player.hpp"

struct GroupUpdate {
    sf::Uint32 group_id;
    bool is_active;
    float x_pos;
    float y_pos;
    float radius;
    bool joinable;
    sf::Vector2f velocity;
};

sf::Packet& operator<<(sf::Packet& packet, const GroupUpdate& group_update);
sf::Packet& operator>>(sf::Packet& packet, GroupUpdate& group_update);

class Group : public CircleGameObject {
  public:
    Group(uint32_t id, sf::Vector2f position, PhysicsController& pc);
    ~Group();
    Group(const Group& temp_obj) = delete;            // TODO: define this
    Group& operator=(const Group& temp_obj) = delete; // TODO: define this

    void setDirection(sf::Vector2f direction);
    void setActive(bool active);

    GroupUpdate getUpdate();
    void applyUpdate(GroupUpdate gu);
    void applyInterpolatedUpdate(GroupUpdate gu, float a, sf::Int32 delta_ms);

  private:
};

#endif /* Group_hpp */
