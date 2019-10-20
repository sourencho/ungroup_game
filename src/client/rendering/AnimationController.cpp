#include <iostream>
#include <algorithm>

#include "AnimationController.hpp"


AnimationController::AnimationController(): mResourceStore() {}

void AnimationController::load() {
    mResourceStore.addTexture("test", "resources/images/spritesheet_test-01.png");
    auto animated_sprite = std::unique_ptr<AnimatedSprite>(
        new AnimatedSprite(mResourceStore.getTexture("test"), {3, 3}, 9000, {100.f, 300.f}));

    add(std::move(animated_sprite));
}

void AnimationController::add(std::unique_ptr<AnimatedSprite> animated_sprite) {
    mAnimatedSprites.push_back(std::move(animated_sprite));
}

void AnimationController::step(sf::Uint32 delta_ms) {
    for (auto& animated_sprite : mAnimatedSprites) {
        animated_sprite->update(delta_ms);
    }
    mAnimatedSprites.erase(std::remove_if(
        mAnimatedSprites.begin(), mAnimatedSprites.end(),
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