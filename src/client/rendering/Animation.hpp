/**
 * ... TODO
 *
 * Inspired by
 * https://github.com/SFML/SFML/wiki/Easy-Animations-With-Spritesheets
 */

#ifndef Animation_hpp
#define Animation_hpp

#include <SFML/Graphics.hpp>

struct Frame {
    sf::IntRect rect;
    sf::Int32 duration; // in milliseconnds
};

class Animation {
  public:
    Animation(sf::Sprite& target);
    ~Animation(){};
    void addFrame(Frame&& frame);
    void update(sf::Int32 elapsed);
    const sf::Int32 getLength() const {
        return m_length;
    };
    bool isDone();

  private:
    sf::Int32 m_length = 0;
    sf::Int32 m_progress = 0;
    std::vector<Frame> m_frames;
    sf::Sprite& m_target;
};

#endif /* Animation_hpp */