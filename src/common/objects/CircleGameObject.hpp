#ifndef CircleGameObject_hpp
#define CircleGameObject_hpp

#include <iostream>
#include <memory>
#include <vector>

#include "../physics/PhysicsController.hpp"
#include "Circle.hpp"
#include "CircleRigidBody.hpp"
#include "GameObject.hpp"

class CircleGameObject : public GameObject {
  public:
    CircleGameObject(uint32_t id, sf::Vector2f position, float radius, sf::Color color,
                     std::shared_ptr<PhysicsController> pc, float mass, bool movable = true);
    Circle& getCircle();
    void setActive(bool is_active);
    void setRadius(float radius);
    void setMass(float mass);
    void setPosition(sf::Vector2f position);
    void applyInput(sf::Vector2f input);
    void matchRigid();
    void setShader(std::shared_ptr<sf::Shader> shader);
    void draw(sf::RenderTarget& render_target);

  protected:
    std::unique_ptr<Circle> mCircle;
    CircleRigidBody& mCircleRigidBody;
    std::shared_ptr<sf::Shader> mShader = nullptr;
};

#endif /* CircleGameObject_hpp */
