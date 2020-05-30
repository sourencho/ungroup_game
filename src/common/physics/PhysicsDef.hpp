#ifndef PhysicsDef_hpp
#define PhysicsDef_hpp

#include "../util/game_settings.hpp"
#include <SFML/Graphics.hpp>

namespace PhysicsDef {

    const float IMPULSE_MILTIPLIER = 3.f;
    const float TRANSITION_SPEED = 20.f; // drag / friction
    const float PLAYER_VELOCITY = GAME_SETTINGS.GROUP_SPEED;
    const float MAX_VELOCITY = PLAYER_VELOCITY * 4.f;
    const float MIN_VELOCITY = -MAX_VELOCITY;
}; // namespace PhysicsDef

struct Impulse {
    float magnitude;
    sf::Vector2f normal;
};

/*
 * Details of collision between two rigid bodies.
 * @ids: ids of the two rigid bodies.
 * @position: point of contact between the rigid bodies after collision is resolved.
 * @normal: normal of the collision from the latter body to the former.
 * @resolution: the amount by which the bodies should be displaced to resolve the collision.
 */
struct Collision {
    std::pair<uint32_t, uint32_t> ids;
    sf::Vector2f position;
    sf::Vector2f normal;
    std::pair<sf::Vector2f, sf::Vector2f> resolution;
    bool collided;
};

sf::Packet& operator<<(sf::Packet& packet, const Collision& collision);
sf::Packet& operator>>(sf::Packet& packet, Collision& collision);

#endif /* PhysicsDef_hpp */
