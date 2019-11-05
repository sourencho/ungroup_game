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
};

sf::Packet& operator<<(sf::Packet& packet, const PlayerUpdate& player_update);
sf::Packet& operator>>(sf::Packet& packet, PlayerUpdate& player_update);

class Player : public GameObject {
  public:
    explicit Player(uint32_t id);
    ~Player();

    void setDirection(sf::Vector2f direction);
    void setJoinable(bool joinable);
    sf::Vector2f getDirection() const;
    bool getJoinable() const;

    PlayerUpdate getUpdate() const;
    void applyUpdate(PlayerUpdate pu);

  private:
    sf::Vector2f m_Direction;
    bool m_Joinable = false;
};

#endif /* Player_hpp */
