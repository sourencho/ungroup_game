#ifndef Player_hpp
#define Player_hpp

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <stdio.h>

#include "../systems/ResourceController.hpp"
#include "../models/WinCondition.hpp"
#include "GameObject.hpp"

struct PlayerUpdate {
    sf::Uint32 player_id;
    bool is_active;
    sf::Vector2f direction;
    bool joinable;
    bool ungroup;
    WinCondition win_condition;
};

sf::Packet& operator<<(sf::Packet& packet, const PlayerUpdate& player_update);
sf::Packet& operator>>(sf::Packet& packet, PlayerUpdate& player_update);

class Player : public GameObject {
  public:
    explicit Player(uint32_t id);
    ~Player();

    void setDirection(sf::Vector2f direction) {
        m_direction = direction;
    };
    sf::Vector2f getDirection() const {
        return m_direction;
    };
    void setJoinable(bool joinable) {
        m_joinable = joinable;
    };
    bool getJoinable() const {
        return m_joinable;
    };
    void setUngroup(bool ungroup) {
        m_ungroup = ungroup;
    };
    bool getUngroup() const {
        return m_ungroup;
    };
    void setWinCondition(WinCondition wc) {
        m_win_condition = wc;
    };
    WinCondition getWinCondition() const {
        return m_win_condition;
    };
    ResourceType getIntent() const {
        return m_intent;
    }

    void toggleUngroup(bool toggle) {
        m_ungroup ^= toggle;
    }

    void toggleJoinable(bool toggle) {
        m_joinable ^= toggle;
    }

    void toggleIntent(bool toggle) {
        if (toggle) {
            m_intent = ResourceType((m_intent + 1) % RESOURCE_TYPE_COUNT);
        }
    }

    PlayerUpdate getUpdate() const;
    void applyUpdate(PlayerUpdate pu);

  private:
    sf::Vector2f m_direction;
    bool m_joinable = false;                   // If player is joinable into a group.
    bool m_ungroup = false;                    // If player wants to ungroup.
    ResourceType m_intent = ResourceType::RED; // Resource player intends to collect. This is
                                               // communicated to other players.
    WinCondition m_win_condition;
};

#endif /* Player_hpp */
