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
                     PhysicsController& pc, ResourceStore& rs, float mass, bool movable = true);

    void applyInput(sf::Vector2f input);

    void setActive(bool is_active);

    void setMass(float mass) {
        m_circleRigidBody.setMass(mass);
    }

    void matchRigid();

    void setShader(RenderingDef::ShaderKey shader_key);

    void setTexture(RenderingDef::TextureKey texture_key);

    sf::Vector2f getCenter() const;

    float getRadius() const {
        return m_circleShape.getRadius();
    };
    void setRadius(float radius) {
        m_circleShape.setRadius(radius);
        m_circleRigidBody.setRadius(radius);
        m_outlineShape.setRadius(radius);
    };

    sf::Vector2f getPosition() const {
        return m_circleShape.getPosition();
    };
    void setPosition(sf::Vector2f position) {
        m_circleShape.setPosition(position);
        m_circleRigidBody.setPosition(position);
        m_outlineShape.setPosition(position);
    };

    void setCenterPosition(sf::Vector2f position) {
        setPosition({position.x - getRadius(), position.y - getRadius()});
    }

    void setOutlineThickness(float thickness) {
        m_outlineShape.setOutlineThickness(thickness);
    };
    void setOutlineColor(sf::Color color) {
        m_outlineShape.setOutlineColor(color);
    };
    void setColor(sf::Color color) {
        m_circleShape.setFillColor(color);
    }

    void draw(sf::RenderTarget& render_target);

    sf::Vector2f getVelocity() const;

  protected:
    sf::CircleShape m_circleShape;
    sf::CircleShape
        m_outlineShape; // We need another circle hidden behind to draw the outline on. We can't use
                        // the original circle because the shader draw over the outline.
    CircleRigidBody& m_circleRigidBody;
    RenderingDef::Shader m_shader;
    ResourceStore& m_resourceStore;
};

#endif /* CircleGameObject_hpp */
