#include "CircleGameObject.hpp"

sf::Clock shader_clock;

CircleGameObject::CircleGameObject(uint32_t id, sf::Vector2f position, float radius,
                                   sf::Color color, std::shared_ptr<PhysicsController> pc,
                                   ResourceStore& rs, float mass, bool movable)
    : GameObject(id), m_circleShape(radius, RenderingDef::CIRCLE_POINT_COUNT),
      m_outlineShape(radius, RenderingDef::CIRCLE_POINT_COUNT),
      m_circleRigidBody(pc->add(std::move(std::unique_ptr<CircleRigidBody>(
          new CircleRigidBody(id, radius, position, mass, movable))))),
      m_resourceStore(rs) {
    m_circleShape.setPosition(position);
    m_circleShape.setFillColor(color);
    m_outlineShape.setPosition(position);
    m_outlineShape.setFillColor(sf::Color::Transparent);
    shader_clock.restart();
};

void CircleGameObject::setActive(bool is_active) {
    m_isActive = is_active;
    m_circleRigidBody.setActive(is_active);
}

void CircleGameObject::applyInput(sf::Vector2f input) { m_circleRigidBody.applyInput(input); }

void CircleGameObject::matchRigid() { setPosition(m_circleRigidBody.getPosition()); }

sf::Vector2f CircleGameObject::getCenter() const {
    sf::Vector2f position = m_circleShape.getPosition();
    float radius = m_circleShape.getRadius();
    return sf::Vector2f(position.x + radius, position.y + radius);
}

void CircleGameObject::draw(sf::RenderTarget& render_target) {
    if (m_isActive) {
        render_target.draw(m_outlineShape);
        if (m_shader.shader != nullptr && RenderingDef::USE_SHADERS) {
            m_shader.shader->setUniform("u_position", getPosition());
            m_shader.shader->setUniform("u_radius", getRadius());
            m_shader.shader->setUniform("u_time", shader_clock.getElapsedTime().asSeconds());
            render_target.draw(m_circleShape, m_shader.shader.get());
        } else {
            render_target.draw(m_circleShape);
        }
    };
}

void CircleGameObject::setShader(RenderingDef::ShaderKey shader_key) {
    m_shader.key = shader_key;
    m_shader.shader = m_resourceStore.getShader(shader_key);
};

void CircleGameObject::setTexture(std::string texture_key) {
    m_circleShape.setTexture(m_resourceStore.getTexture(texture_key).get());
}