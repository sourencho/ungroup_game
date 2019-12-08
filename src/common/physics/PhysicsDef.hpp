#ifndef PhysicsDef_hpp
#define PhysicsDef_hpp

#include <SFML/Graphics.hpp>

namespace PhysicsDef {
    struct Impulse {
        float magnitude;
        sf::Vector2f normal;
    };
    const float IMPULSE_MILTIPLIER = 3.f;
    const float TRANSITION_SPEED = 10.f; // drag / friction
    const float PLAYER_VELOCITY = 200.f;
    const float MAX_VELOCITY = PLAYER_VELOCITY * 4.f;
    const float MIN_VELOCITY = -MAX_VELOCITY;
}; // namespace PhysicsDef

#endif /* PhysicsDef_hpp */