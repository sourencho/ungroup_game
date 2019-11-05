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

    void setMass(float mass) { m_CircleRigidBody.setMass(mass); }

    void matchRigid();

    void setShader(RenderingDef::ShaderKey shader_key);

    void setTexture(std::string texture_key);

    sf::Vector2f getCenter() const;

    float getRadius() const { return m_CircleShape.getRadius(); };
    void setRadius(float radius) {
        m_CircleShape.setRadius(radius);
        m_CircleRigidBody.setRadius(radius);
        m_OutlineShape.setRadius(radius);
    };

    sf::Vector2f getPosition() const { return m_CircleShape.getPosition(); };
    void setPosition(sf::Vector2f position) {
        m_CircleShape.setPosition(position);
        m_CircleRigidBody.setPosition(position);
        m_OutlineShape.setPosition(position);
    };

    void setOutlineThickness(float thickness) { m_OutlineShape.setOutlineThickness(thickness); };
    void setOutlineColor(sf::Color color) { m_OutlineShape.setOutlineColor(color); };

    void draw(sf::RenderTarget& render_target);

    void setOutline(){};

  protected:
    sf::CircleShape m_CircleShape;
    sf::CircleShape
        m_OutlineShape; // We need another circle hidden behind to draw the outline on. We can't use
                       // the original circle because the shader draw over the outline.
    CircleRigidBody& m_CircleRigidBody;
    RenderingDef::Shader m_Shader;
    ResourceStore& m_ResourceStore;
};

#endif /* CircleGameObject_hpp */
