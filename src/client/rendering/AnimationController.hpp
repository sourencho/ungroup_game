#ifndef AnimationController_hpp
#define AnimationController_hpp

#include "AnimatedSprite.hpp"

class AnimationController {
  public:
    explicit AnimationController(){};
    ~AnimationController(){};
    AnimationController(const AnimationController& temp_obj) = delete;
    AnimationController& operator=(const AnimationController& temp_obj) = delete;

    void add(std::unique_ptr<AnimatedSprite> animated_sprite);
    void step(sf::Uint32 delta_ms);
    void draw(sf::RenderTarget& target);
    const size_t getCount() const { return mAnimatedSprites.size(); }

  private:
    std::vector<std::unique_ptr<AnimatedSprite>> mAnimatedSprites;
};

#endif /* AnimationController_hpp */
