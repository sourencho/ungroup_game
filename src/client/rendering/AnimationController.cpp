#include <algorithm>
#include <iostream>

#include "AnimationController.hpp"

void AnimationController::add(std::unique_ptr<AnimatedSprite> animated_sprite) {
    mAnimatedSprites.push_back(std::move(animated_sprite));
}

void AnimationController::step(sf::Uint32 delta_ms) {
    for (auto& animated_sprite : mAnimatedSprites) {
        animated_sprite->update(delta_ms);
    }
    mAnimatedSprites.erase(
        std::remove_if(mAnimatedSprites.begin(), mAnimatedSprites.end(),
                       [](const std::unique_ptr<AnimatedSprite>& animated_sprite) {
                           return animated_sprite->isDone();
                       }),
        mAnimatedSprites.end());
}

void AnimationController::draw(sf::RenderTarget& target) {
    for (auto& animated_sprite : mAnimatedSprites) {
        animated_sprite->draw(target);
    }
}