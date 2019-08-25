#ifndef Mine_hpp
#define Mine_hpp

#include <stdio.h>
#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>
#include "../common/Circle.hpp"
#include "../common/CircleGameObject.hpp"

class Mine : public CircleGameObject {
 public:
     Mine(unsigned int id, sf::Vector2f position, float size, sf::Color color,
        std::shared_ptr<PhysicsController> pc);
     ~Mine();
     Mine(const Mine& temp_obj) = delete;  // TODO: define this
     Mine& operator=(const Mine& temp_obj) = delete;  // TODO: define this
};

#endif /* Mine_hpp */
