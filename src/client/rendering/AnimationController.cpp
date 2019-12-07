#include "AnimationController.hpp"

#include <algorithm>
#include <iostream>

#include "../../common/events/CollisionEvent.hpp"
#include "../../common/events/EventController.hpp"
#include "../../common/rendering/RenderingDef.hpp"

AnimationController::AnimationController(ResourceStore& rs) : m_resourceStore(rs) {
    addEventListeners();
}

void AnimationController::add(std::unique_ptr<AnimatedSprite> animated_sprite) {
    m_animatedSprites.push_back(std::move(animated_sprite));
}

void AnimationController::update(sf::Uint32 delta_ms) {
    for (auto& animated_sprite : m_animatedSprites) {
        animated_sprite->update(delta_ms);
    }
    m_animatedSprites.erase(
        std::remove_if(m_animatedSprites.begin(), m_animatedSprites.end(),
                       [](const std::unique_ptr<AnimatedSprite>& animated_sprite) {
                           return animated_sprite->isDone();
                       }),
        m_animatedSprites.end());
}

void AnimationController::draw(sf::RenderTarget& target) {
    for (auto& animated_sprite : m_animatedSprites) {
        animated_sprite->draw(target);
    }
}

void AnimationController::addEventListeners() {
    EventController::getInstance().addEventListener(
        EventType::EVENT_TYPE_COLLISION,
        std::bind(&AnimationController::handleCollisionEvent, this, std::placeholders::_1));
}

void AnimationController::handleCollisionEvent(std::shared_ptr<Event> event) {
    std::shared_ptr<CollisionEvent> collision_event =
        std::dynamic_pointer_cast<CollisionEvent>(event);

    createCollisionAnimation(collision_event->getCollision().position);
}

void AnimationController::createCollisionAnimation(const sf::Vector2f& collision_position) {
    auto collision_sprite = std::unique_ptr<AnimatedSprite>(new AnimatedSprite(
        *m_resourceStore.getTexture(RenderingDef::TextureKey::collision), {6, 1}, 240,
        collision_position, {2.f, 2.f}, RenderingDef::COLLISION_ANIMATION_COLOR));
    add(std::move(collision_sprite));
}