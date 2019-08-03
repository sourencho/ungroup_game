#ifndef PhysicsController_hpp
#define PhysicsController_hpp

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include "CircleRigidBody.hpp"

class PhysicsController{
 public:
     PhysicsController();
     void addCircleRigidBody(std::shared_ptr<CircleRigidBody> crb);
     void step();
     void handleCollision();
 private:
     std::vector<std::shared_ptr<CircleRigidBody>> mCircleRigidBodies;
};

#endif /* PhysicsController_hpp */
