#include <algorithm>
#include <iostream>

#include "AnimationController.hpp"

void AnimationController::add(std::unique_ptr<AnimatedSprite> animated_sprite) {
    m_animatedSprites.push_back(std::move(animated_sprite));
}

void AnimationController::step(sf::Uint32 delta_ms) {
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