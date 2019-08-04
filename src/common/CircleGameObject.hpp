#ifndef CircleGameObject_hpp
#define CircleGameObject_hpp

#include <vector>
#include <memory>
#include <iostream>

#include "GameObject.hpp"
#include "../common/Circle.hpp"
#include "../common/CircleRigidBody.hpp"
#include "../common/PhysicsController.hpp"

class CircleGameObject : public GameObject {
 public:
     CircleGameObject(
        unsigned int id, sf::Vector2f position, float radius, std::shared_ptr<CircleRigidBody> crb);
     std::shared_ptr<Circle> getCircle();
     std::shared_ptr<CircleRigidBody> getCircleRigidBody();
     void setActive(bool is_active);
     void setVelocity(sf::Vector2f velocity);
     void setRadius(float radius);
     void setPosition(sf::Vector2f position);
     void matchRigid();
 protected:
     std::shared_ptr<Circle> mCircle;
     std::shared_ptr<CircleRigidBody> mCircleRigidBody;
};

#endif /* CircleGameObject_hpp */
