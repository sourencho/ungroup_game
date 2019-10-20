#include "AnimatedSprite.hpp"

/**
 * Creates an animated sprite from a spritesheet.
 *
 * @spritesheet_texutre: The texture containing the spritesheet.
 * The spritesheet grid needs to be a full and comprised of equally sized
 * sprites.
 * @spritesheet_size: The size of the sprite grid in number of sprites.
 * @duration: The totol duration of the animation.
 * Each sprite will show for an equal amount of time.
 * @position: Position of the animation sprite.
 */
AnimatedSprite::AnimatedSprite(sf::Texture &spritesheet_texture, sf::Vector2u spritesheet_size,
                               sf::Int32 duration_ms, sf::Vector2f position)
    : mSprite(spritesheet_texture), mAnimation(mSprite) {
    mSprite.setPosition(position);
    sf::Int32 frame_count = spritesheet_size.x * spritesheet_size.y;
    sf::Vector2u texture_size = spritesheet_texture.getSize();
    uint frame_width = texture_size.x / spritesheet_size.x;
    uint frame_height = texture_size.y / spritesheet_size.y;
    for (size_t i = 0; i < spritesheet_size.x; i++) {
        for (size_t j = 0; j < spritesheet_size.y; j++) {
            auto position = sf::Vector2i(i * frame_width, j * frame_height);
            auto size = sf::Vector2i(frame_width, frame_height);
            mAnimation.addFrame({sf::IntRect(position, size), duration_ms / frame_count});
        }
    }
}

void AnimatedSprite::draw(sf::RenderTarget &target) { target.draw(mSprite); }

void AnimatedSprite::update(sf::Uint32 delta_ms) { mAnimation.update(delta_ms); }

bool AnimatedSprite::isDone() { return mAnimation.isDone(); }
