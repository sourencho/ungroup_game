#ifndef CircleGameObject_hpp
#define CircleGameObject_hpp

#include <iostream>
#include <memory>
#include <vector>

#include "../physics/PhysicsController.hpp"
#include "../rendering/RenderingDef.hpp"
#include "../resources/ResourceStore.hpp"
#include "CircleRigidBody.hpp"
#include "GameObject.hpp"

class CircleGameObject : public GameObject {
  public:
    CircleGameObject(uint32_t id, sf::Vector2f position, float radius, sf::Color color,
                     std::shared_ptr<PhysicsController> pc, ResourceStore& rs, float mass,
                     bool movable = true);

    void applyInput(sf::Vector2f input);

    void setActive(bool is_active);

    void setMass(float mass) { mCircleRigidBody.setMass(mass); }

    void matchRigid();

    void setShader(RenderingDef::ShaderKey shader_key);

    void setTexture(std::string texture_key);

    sf::Vector2f getCenter() const;

    float getRadius() const { return mCircleShape->getRadius(); };
    void setRadius(float radius) {
        mCircleShape->setRadius(radius);
        mCircleRigidBody.setRadius(radius);
    };

    sf::Vector2f getPosition() const { return mCircleShape->getPosition(); };
    void setPosition(sf::Vector2f position) {
        mCircleShape->setPosition(position);
        mCircleRigidBody.setPosition(position);
    };

    void draw(sf::RenderTarget& target);

    sf::CircleShape& getCircleShape() { return *mCircleShape; }

  protected:
    std::unique_ptr<sf::CircleShape> mCircleShape;
    CircleRigidBody& mCircleRigidBody;
    RenderingDef::Shader mShader;
    ResourceStore& mResourceStore;
};

#endif /* CircleGameObject_hpp */
