#include "CircleGameObject.hpp"

sf::Clock shader_clock;

CircleGameObject::CircleGameObject(uint32_t id, sf::Vector2f position, float radius,
                                   sf::Color color, std::shared_ptr<PhysicsController> pc,
                                   ResourceStore& rs, float mass, bool movable)
    : GameObject(id), m_CircleShape(radius, RenderingDef::CIRCLE_POINT_COUNT),
      m_OutlineShape(radius, RenderingDef::CIRCLE_POINT_COUNT),
      m_CircleRigidBody(pc->add(std::move(std::unique_ptr<CircleRigidBody>(
          new CircleRigidBody(id, radius, position, mass, movable))))),
      m_ResourceStore(rs) {
    m_CircleShape.setPosition(position);
    m_CircleShape.setFillColor(color);
    m_OutlineShape.setPosition(position);
    m_OutlineShape.setFillColor(sf::Color::Transparent);
    shader_clock.restart();
};

void CircleGameObject::setActive(bool is_active) {
    m_IsActive = is_active;
    m_CircleRigidBody.setActive(is_active);
}

void CircleGameObject::applyInput(sf::Vector2f input) { m_CircleRigidBody.applyInput(input); }

void CircleGameObject::matchRigid() { setPosition(m_CircleRigidBody.getPosition()); }

sf::Vector2f CircleGameObject::getCenter() const {
    sf::Vector2f position = m_CircleShape.getPosition();
    float radius = m_CircleShape.getRadius();
    return sf::Vector2f(position.x + radius, position.y + radius);
}

void CircleGameObject::draw(sf::RenderTarget& render_target) {
    if (m_IsActive) {
        render_target.draw(m_OutlineShape);
        if (m_Shader.shader != nullptr && RenderingDef::USE_SHADERS) {
            m_Shader.shader->setUniform("u_position", getPosition());
            m_Shader.shader->setUniform("u_radius", getRadius());
            m_Shader.shader->setUniform("u_time", shader_clock.getElapsedTime().asSeconds());
            render_target.draw(m_CircleShape, m_Shader.shader.get());
        } else {
            render_target.draw(m_CircleShape);
        }
    };
}

void CircleGameObject::setShader(RenderingDef::ShaderKey shader_key) {
    m_Shader.key = shader_key;
    m_Shader.shader = m_ResourceStore.getShader(shader_key);
};

void CircleGameObject::setTexture(std::string texture_key) {
    m_CircleShape.setTexture(m_ResourceStore.getTexture(texture_key).get());
}