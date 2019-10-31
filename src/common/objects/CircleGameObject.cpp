#include "CircleGameObject.hpp"

CircleGameObject::CircleGameObject(uint32_t id, sf::Vector2f position, float radius,
                                   sf::Color color, std::shared_ptr<PhysicsController> pc,
                                   ResourceStore& rs, float mass, bool movable)
    : GameObject(id), mCircle(std::unique_ptr<Circle>(new Circle(radius, position, color))),
      mCircleRigidBody(pc->add(std::move(std::unique_ptr<CircleRigidBody>(
          new CircleRigidBody(id, radius, position, mass, movable))))),
      mResourceStore(rs){};

Circle& CircleGameObject::getCircle() { return *mCircle; }

void CircleGameObject::setActive(bool is_active) {
    mIsActive = is_active;
    mCircleRigidBody.setActive(is_active);
}

void CircleGameObject::applyInput(sf::Vector2f input) { mCircleRigidBody.applyInput(input); }

void CircleGameObject::setRadius(float radius) {
    mCircle->setRadius(radius);
    mCircleRigidBody.setRadius(radius);
}

void CircleGameObject::setMass(float mass) { mCircleRigidBody.setMass(mass); }

void CircleGameObject::setPosition(sf::Vector2f position) {
    mCircle->setPosition(position);
    mCircleRigidBody.setPosition(position);
}

void CircleGameObject::matchRigid() { mCircle->setPosition(mCircleRigidBody.getPosition()); }

void CircleGameObject::draw(sf::RenderTarget& render_target) {
    if (mIsActive) {
        if (mShader.shader != nullptr) {
            mCircle->draw(render_target, *mShader.shader);
        } else {
            mCircle->draw(render_target);
        }
    };
}

void CircleGameObject::setShader(RenderingDef::ShaderKey shader_key) {
    mShader.key = shader_key;
    mShader.shader = mResourceStore.getShader(shader_key);
};