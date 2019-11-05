#ifndef PhysicsController_hpp
#define PhysicsController_hpp

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "../objects/CircleRigidBody.hpp"
#include "CollisionUtil.hpp"

class PhysicsController {
  public:
    PhysicsController();
    CircleRigidBody& add(std::unique_ptr<CircleRigidBody> circle_rigid_body);
    void update(sf::Int32 delta_ms);
    void step(sf::Int32 delta_ms);

  private:
    void resolveCollisions();
    void resolveCollision(CircleRigidBody& circle_a, CircleRigidBody& circle_b,
                          const Collision& collision);
    void fireCollisionEvent(const Collision& collision);

    std::vector<std::unique_ptr<CircleRigidBody>> m_CircleRigidBodies;
};

#endif /* PhysicsController_hpp */
