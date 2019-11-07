#ifndef Player_hpp
#define Player_hpp

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <stdio.h>

#include "GameObject.hpp"

struct PlayerUpdate {
    sf::Uint32 player_id;
    bool is_active;
    sf::Vector2f direction;
    bool joinable;
    bool ungroup;
};

sf::Packet& operator<<(sf::Packet& packet, const PlayerUpdate& player_update);
sf::Packet& operator>>(sf::Packet& packet, PlayerUpdate& player_update);

class Player : public GameObject {
  public:
    explicit Player(uint32_t id);
    ~Player();

    void setDirection(sf::Vector2f direction) { m_direction = direction; };
    sf::Vector2f getDirection() const { return m_direction; };
    void setJoinable(bool joinable) { m_joinable = joinable; };
    bool getJoinable() const { return m_joinable; };
    void setUngroup(bool ungroup) { m_ungroup = ungroup; };
    bool getUngroup() const { return m_ungroup; };

    PlayerUpdate getUpdate() const;
    void applyUpdate(PlayerUpdate pu);

  private:
    sf::Vector2f m_direction;
    bool m_joinable = false;
    bool m_ungroup = false;
};

#endif /* Player_hpp */
