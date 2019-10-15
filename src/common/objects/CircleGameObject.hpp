#ifndef CircleGameObject_hpp
#define CircleGameObject_hpp

#include <vector>
#include <memory>
#include <iostream>

#include "GameObject.hpp"
#include "Circle.hpp"
#include "CircleRigidBody.hpp"
#include "../systems/PhysicsController.hpp"

class CircleGameObject : public GameObject {
 public:
     CircleGameObject(uint32_t id, sf::Vector2f position, float radius, sf::Color color,
        std::shared_ptr<PhysicsController> pc);
     std::shared_ptr<Circle> getCircle();
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
