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
    Animation(sf::Sprite &target);
    ~Animation(){};
    void addFrame(Frame &&frame);
    void update(sf::Int32 elapsed);
    const sf::Int32 getLength() const { return mLength; };
    bool isDone();

  private:
    sf::Int32 mLength = 0;
    sf::Int32 mProgress = 0;
    std::vector<Frame> mFrames;
    sf::Sprite &mTarget;
};

#endif /* Animation_hpp */