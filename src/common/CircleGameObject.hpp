#ifndef CircleGameObject_hpp
#define CircleGameObject_hpp

#include <vector>
#include <memory>
#include <iostream>

#include "GameObject.hpp"
#include "../common/Circle.hpp"
#include "../common/CircleRigidBody.hpp"

class CircleGameObject : public GameObject {
 public:
     CircleGameObject(unsigned int id, sf::Vector2f position, float radius);
     std::shared_ptr<Circle> getCircle();
     std::shared_ptr<CircleRigidBody> getCircleRigidBody();
     static std::vector<std::shared_ptr<Circle>> getCircles(
         std::vector<std::shared_ptr<CircleGameObject>>& circle_game_objects);
     void setActive(bool is_active);
     void setVelocity(sf::Vector2f velocity);
     void setRadius(float radius);
     void matchRigid();
 protected:
     std::shared_ptr<Circle> mCircle;
     std::shared_ptr<CircleRigidBody> mCircleRigidBody;
};

#endif /* CircleGameObject_hpp */
