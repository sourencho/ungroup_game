#ifndef Group_hpp
#define Group_hpp

#include <stdio.h>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Player.hpp"
#include "Circle.hpp"
#include "CircleGameObject.hpp"
#include "../systems/PhysicsController.hpp"


struct GroupUpdate {
    sf::Uint32 group_id;
    bool is_active;
    float x_pos;
    float y_pos;
    float radius;
    bool groupable;
    sf::Uint32 members_size;
    std::vector<sf::Uint32> members;
};

sf::Packet& operator <<(sf::Packet& packet, const GroupUpdate& group_update);
sf::Packet& operator >>(sf::Packet& packet, GroupUpdate& group_update);

class Group : public CircleGameObject {
 public:
    Group(int id, sf::Vector2f position, sf::Color color, std::shared_ptr<PhysicsController> pc,
        const std::vector<std::shared_ptr<Player>>& players);
    ~Group();
    Group(const Group& temp_obj) = delete;  // TODO: define this
    Group& operator=(const Group& temp_obj) = delete;  // TODO: define this

    void update();
    bool getGroupable();
    void setGroupable(bool groupable);
    void addMember(int player_id);

    GroupUpdate getUpdate();
    void applyUpdate(GroupUpdate gu);

 private:
    std::vector<int> mMembers;  // vector of player ids
    void refresh();

    bool mGroupable = false;
    std::vector<std::shared_ptr<Player>> mPlayers;  // TODO(souren): Fetch players by id from manager
};

#endif /* Group_hpp */
