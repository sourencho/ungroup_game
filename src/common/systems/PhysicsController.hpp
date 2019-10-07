#ifndef PhysicsController_hpp
#define PhysicsController_hpp

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include "../objects/CircleRigidBody.hpp"


class PhysicsController{
 public:
     PhysicsController();
     std::shared_ptr<CircleRigidBody> createCRB(float radius, sf::Vector2f position);
     void update(sf::Int32 delta_ms);
     void step(sf::Int32 delta_ms);
     void handleCollision();
 private:
     std::vector<std::shared_ptr<CircleRigidBody>> mCircleRigidBodies;
};

#endif /* PhysicsController_hpp */
