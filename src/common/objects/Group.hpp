#ifndef Group_hpp
#define Group_hpp

#include <memory>
#include <stdio.h>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "../physics/PhysicsController.hpp"
#include "../rendering/DirectionArrows.hpp"
#include "../rendering/DirectionLines.hpp"
#include "../resources/ResourceStore.hpp"
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

    void setDirection(sf::Vector2f direction);
    void setActive(bool active);

    GroupUpdate getUpdate();
    void applyUpdate(GroupUpdate gu);

    void draw(sf::RenderTarget& render_target, size_t player_count, bool joinable, bool ungroup,
              std::vector<sf::Vector2f> player_directions, std::vector<ResourceType> player_intents,
              std::array<uint32_t, RESOURCE_TYPE_COUNT> resource_counts);

  private:
    DirectionArrows m_directionArrow;
    DirectionLines m_directionLines;
    float m_resourceCounts[RESOURCE_TYPE_COUNT] = {0}; // Used to pass resource counts to shader
};

#endif /* Group_hpp */
