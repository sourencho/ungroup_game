#include "CircleGameObject.hpp"

sf::Clock shader_clock;

CircleGameObject::CircleGameObject(uint32_t id, sf::Vector2f position, float radius,
                                   sf::Color color, std::shared_ptr<PhysicsController> pc,
                                   ResourceStore& rs, float mass, bool movable)
    : GameObject(id), mCircleShape(std::unique_ptr<sf::CircleShape>(
                          new sf::CircleShape(radius, RenderingDef::CIRCLE_POINT_COUNT))),
      mCircleRigidBody(pc->add(std::move(std::unique_ptr<CircleRigidBody>(
          new CircleRigidBody(id, radius, position, mass, movable))))),
      mResourceStore(rs) {
    mCircleShape->setPosition(position);
    mCircleShape->setFillColor(color);
    shader_clock.restart();
};

void CircleGameObject::setActive(bool is_active) {
    mIsActive = is_active;
    mCircleRigidBody.setActive(is_active);
}

void CircleGameObject::applyInput(sf::Vector2f input) { mCircleRigidBody.applyInput(input); }

void CircleGameObject::matchRigid() { mCircleShape->setPosition(mCircleRigidBody.getPosition()); }

sf::Vector2f CircleGameObject::getCenter() const {
    sf::Vector2f position = mCircleShape->getPosition();
    float radius = mCircleShape->getRadius();
    return sf::Vector2f(position.x + radius, position.y + radius);
}

void CircleGameObject::draw(sf::RenderTarget& render_target) {
    if (mIsActive) {
        if (mShader.shader != nullptr && RenderingDef::USE_SHADERS) {
            mShader.shader->setUniform("u_position", getPosition());
            mShader.shader->setUniform("u_radius", getRadius());
            mShader.shader->setUniform("u_time", shader_clock.getElapsedTime().asSeconds());
            render_target.draw(*mCircleShape, mShader.shader.get());
        } else {
            render_target.draw(*mCircleShape);
        }
    };
}

void CircleGameObject::setShader(RenderingDef::ShaderKey shader_key) {
    mShader.key = shader_key;
    mShader.shader = mResourceStore.getShader(shader_key);
};

void CircleGameObject::setTexture(std::string texture_key) {
    mCircleShape->setTexture(mResourceStore.getTexture(texture_key).get());
}