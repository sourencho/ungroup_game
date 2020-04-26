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
AnimatedSprite::AnimatedSprite(sf::Texture& spritesheet_texture, sf::Vector2u spritesheet_size,
                               sf::Int32 duration_ms, sf::Vector2f position,
                               sf::Vector2f scale_factor, sf::Color color) :
    m_sprite(spritesheet_texture),
    m_animation(m_sprite) {
    sf::Int32 frame_count = spritesheet_size.x * spritesheet_size.y;
    sf::Vector2u texture_size = spritesheet_texture.getSize();
    uint32_t frame_width = texture_size.x / spritesheet_size.x;
    uint32_t frame_height = texture_size.y / spritesheet_size.y;
    auto size = sf::Vector2i(frame_width, frame_height);
    for (size_t j = 0; j < spritesheet_size.y; j++) {
        for (size_t i = 0; i < spritesheet_size.x; i++) {
            auto position = sf::Vector2i(i * frame_width, j * frame_height);
            m_animation.addFrame({sf::IntRect(position, size), duration_ms / frame_count});
        }
    }
    m_sprite.setColor(color);
    m_sprite.scale(scale_factor);
    m_sprite.setOrigin({frame_width / 2.f, frame_height / 2.f});
    m_sprite.setPosition(position);
}

void AnimatedSprite::draw(sf::RenderTarget& target) {
    target.draw(m_sprite);
}

void AnimatedSprite::update(sf::Uint32 delta_ms) {
    m_animation.update(delta_ms);
}

bool AnimatedSprite::isDone() {
    return m_animation.isDone();
}
