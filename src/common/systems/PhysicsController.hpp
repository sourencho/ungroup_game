#ifndef PhysicsController_hpp
#define PhysicsController_hpp

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "../objects/CircleRigidBody.hpp"

class PhysicsController {
  public:
    PhysicsController();
    std::shared_ptr<CircleRigidBody> createCRB(uint32_t id, float radius, sf::Vector2f position);
    void update(sf::Int32 delta_ms);
    void step(sf::Int32 delta_ms);

  private:
    std::vector<std::shared_ptr<CircleRigidBody>> mCircleRigidBodies;
    void handleCollision();
    void fireCollisionEvent(sf::Vector2f position);
};

#endif /* PhysicsController_hpp */
