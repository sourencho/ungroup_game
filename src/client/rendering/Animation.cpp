#include <iostream>

#include "Animation.hpp"

Animation::Animation(sf::Sprite &target) : mTarget(target) {}

void Animation::addFrame(Frame &&frame) {
    mLength += frame.duration;
    mFrames.push_back(std::move(frame));
}

void Animation::update(sf::Int32 elapsed) {
    mProgress += elapsed;
    sf::Int32 p = mProgress;
    for (size_t i = 0; i < mFrames.size(); i++) {
        p -= mFrames[i].duration;

        // if we have progressed OR if we're on the last frame, apply and stop.
        if (p <= 0.0 || &(mFrames[i]) == &mFrames.back()) {
            mTarget.setTextureRect(mFrames[i].rect);
            break; // we found our frame
        }
    }
}

bool Animation::isDone() {
    return mProgress > mLength;
}