#ifndef Group_hpp
#define Group_hpp

#include <memory>
#include <stdio.h>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "../physics/PhysicsController.hpp"
#include "../rendering/DirectionArrows.hpp"
#include "../resources/ResourceStore.hpp"
#include "CircleGameObject.hpp"
#include "Player.hpp"

struct GroupUpdate {
    sf::Uint32 group_id;
    bool is_active;
    float x_pos;
    float y_pos;
    float radius;
    bool joinable;
    sf::Uint32 shader_key;
};

sf::Packet& operator<<(sf::Packet& packet, const GroupUpdate& group_update);
sf::Packet& operator>>(sf::Packet& packet, GroupUpdate& group_update);

class Group : public CircleGameObject {
  public:
    Group(uint32_t id, sf::Vector2f position, sf::Color color, PhysicsController& pc,
          ResourceStore& rs);
    ~Group();
    Group(const Group& temp_obj) = delete;            // TODO: define this
    Group& operator=(const Group& temp_obj) = delete; // TODO: define this

    bool getJoinable() {
        return m_joinable;
    };
    void setJoinable(bool joinable) {
        m_joinable = joinable;
    };
    bool getUngroup() {
        return m_ungroup;
    };
    void setUngroup(bool ungroup) {
        m_ungroup = ungroup;
    };
    void setDirection(sf::Vector2f direction);

    void setTargetDirections(std::vector<sf::Vector2f> target_directions) {
        m_targetDirections = target_directions;
    }

    void setActive(bool active);

    GroupUpdate getUpdate();
    void applyUpdate(GroupUpdate gu);

    void draw(sf::RenderTarget& render_target);

  private:
    bool m_joinable = false;
    bool m_ungroup = false;
    std::vector<sf::Vector2f> m_targetDirections;

    DirectionArrows m_directionArrow;
};

#endif /* Group_hpp */
