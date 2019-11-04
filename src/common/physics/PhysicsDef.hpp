#ifndef PhysicsDef_hpp
#define PhysicsDef_hpp

#include <SFML/Graphics.hpp>

namespace PhysicsDef {
    struct Impulse {
        float magnitude;
        sf::Vector2f normal;
    };
    const float IMPULSE_MILTIPLIER = 3.f;
}; // namespace PhysicsDef

#endif /* PhysicsDef_hpp */