/**
 * TODO
 */

#ifndef AnimatedSprite_hpp
#define AnimatedSprite_hpp

#include <SFML/Graphics.hpp>

#include "Animation.hpp"

class AnimatedSprite {
  public:
    AnimatedSprite(sf::Texture& spritesheet_texture, sf::Vector2u spritesheet_size,
                   sf::Int32 duration_ms, sf::Vector2f position, sf::Vector2f scale_factor);
    ~AnimatedSprite(){};

    void draw(sf::RenderTarget& target);
    void update(sf::Uint32 delta_ms);
    bool isDone();

  private:
    sf::Sprite m_sprite;
    Animation m_animation;
};

#endif /* AnimatedSprite_hpp */