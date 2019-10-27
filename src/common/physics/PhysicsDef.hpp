#ifndef PhysicsDef_hpp
#define PhysicsDef_hpp

#include <SFML/Graphics.hpp>

namespace PhysicsDef {
    struct Impulse {
        float magnitude;
        sf::Vector2f normal;
    };
}; // namespace PhysicsDef

#endif /* PhysicsDef_hpp */