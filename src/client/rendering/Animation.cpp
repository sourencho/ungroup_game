#include <iostream>

#include "Animation.hpp"

Animation::Animation(sf::Sprite& target) : m_Target(target) {}

void Animation::addFrame(Frame&& frame) {
    m_Length += frame.duration;
    m_Frames.push_back(std::move(frame));
}

void Animation::update(sf::Int32 elapsed) {
    m_Progress += elapsed;
    sf::Int32 p = m_Progress;
    for (size_t i = 0; i < m_Frames.size(); i++) {
        p -= m_Frames[i].duration;

        // if we have progressed OR if we're on the last frame, apply and stop.
        if (p <= 0.0 || &(m_Frames[i]) == &m_Frames.back()) {
            m_Target.setTextureRect(m_Frames[i].rect);
            break; // we found our frame
        }
    }
}

bool Animation::isDone() { return m_Progress > m_Length; }