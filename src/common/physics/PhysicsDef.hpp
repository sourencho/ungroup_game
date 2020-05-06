#ifndef PhysicsDef_hpp
#define PhysicsDef_hpp

#include "../util/game_settings.hpp"
#include <SFML/Graphics.hpp>

namespace PhysicsDef {
    struct Impulse {
        float magnitude;
        sf::Vector2f normal;
    };
    const float IMPULSE_MILTIPLIER = 2.f;
    const float TRANSITION_SPEED = 20.f; // drag / friction
    const float PLAYER_VELOCITY = GAME_SETTINGS.GROUP_SPEED;
    const float MAX_VELOCITY = PLAYER_VELOCITY * 4.f;
    const float MIN_VELOCITY = -MAX_VELOCITY;
}; // namespace PhysicsDef

#endif /* PhysicsDef_hpp */