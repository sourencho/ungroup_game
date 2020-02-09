#ifndef Player_hpp
#define Player_hpp

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <stdio.h>

#include "../models/WinCondition.hpp"
#include "../systems/ResourceController.hpp"
#include "GameObject.hpp"

struct PlayerUpdate {
    sf::Uint32 player_id;
    bool is_active;
    sf::Vector2f direction;
    bool joinable;
    bool ungroup;
    std::array<uint32_t, RESOURCE_TYPE_COUNT> resource_counts_to_win;
    sf::Uint32 intent;
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
    void setResourceCountToWin(std::array<uint32_t, RESOURCE_TYPE_COUNT> resource_counts_to_win) {
        m_winCondition.setResourceCountToWin(resource_counts_to_win);
    };
    const WinCondition& getWinCondition() {
        return m_winCondition;
    };
    const std::array<uint32_t, RESOURCE_TYPE_COUNT>& getResourceCountsToWin() const {
        return m_winCondition.getResourceCountsToWin();
    };
    ResourceType getIntent() const {
        return m_intent;
    };
    void setIntent(ResourceType intent) {
        m_intent = intent;
    };
    void toggleUngroup(bool toggle) {
        m_ungroup ^= toggle;
    };
    void toggleJoinable(bool toggle) {
        m_joinable ^= toggle;
    };
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
    WinCondition m_winCondition;
};

#endif /* Player_hpp */
