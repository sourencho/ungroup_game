#include <iostream>

#include "Animation.hpp"

Animation::Animation(sf::Sprite& target) : m_target(target) {
}

void Animation::addFrame(Frame&& frame) {
    m_length += frame.duration;
    m_frames.push_back(std::move(frame));
}

void Animation::update(sf::Int32 elapsed) {
    m_progress += elapsed;
    sf::Int32 p = m_progress;
    for (size_t i = 0; i < m_frames.size(); i++) {
        p -= m_frames[i].duration;

        // if we have progressed OR if we're on the last frame, apply and stop.
        if (p <= 0.0 || &(m_frames[i]) == &m_frames.back()) {
            m_target.setTextureRect(m_frames[i].rect);
            break; // we found our frame
        }
    }
}

bool Animation::isDone() {
    return m_progress > m_length;
}