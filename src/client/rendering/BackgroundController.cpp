#include "BackgroundController.hpp"

#include <iostream>

#include "../../common/rendering/RenderingDef.hpp"
#include "../../common/util/game_settings.hpp"

BackgroundController::BackgroundController(sf::Vector2u background_size, ResourceStore& rs) :
    m_resourceStore(rs), m_backgroundSize(background_size),
    m_boundsCircle(GAME_BOUNDS_RADIUS, RenderingDef::GAME_BOUNDS_CIRCLE_SIDES) {
    m_boundsCircle.setFillColor(RenderingDef::BACKGROUND_COLOR);

    auto dotted_background =
        m_resourceStore.getTexture(RenderingDef::TextureKey::dotted_background);

    m_backgroundSprite = sf::Sprite(*dotted_background);
    m_backgroundSprite2 = sf::Sprite(*dotted_background);

    m_backgroundSprite.setTextureRect(sf::IntRect(0, 0, m_backgroundSize.x, m_backgroundSize.y));
    m_backgroundSprite2.setTextureRect(sf::IntRect(dotted_background->getSize().x / 2.f,
                                                   dotted_background->getSize().y / 2.f,
                                                   m_backgroundSize.x, m_backgroundSize.y));

    m_backgroundSprite.setColor(sf::Color(255, 255, 255, RenderingDef::BACKGROUND_TEXTURE_1_ALPHA));
    m_backgroundSprite2.setColor(
        sf::Color(255, 255, 255, RenderingDef::BACKGROUND_TEXTURE_2_ALPHA));

    m_backgroundSprite.setScale(RenderingDef::BACKGROUND_TEXTURE_1_SCALE);
    m_backgroundSprite2.setScale(RenderingDef::BACKGROUND_TEXTURE_2_SCALE);
}

void BackgroundController::update(sf::Vector2f player_position, sf::Vector2u window_size) {
    // Move texture position relative to player movement to achieve parallax effect
    m_backgroundSprite.setTextureRect(
        sf::IntRect(player_position.x * -RenderingDef::BACKGROUND_TEXUTURE_1_SPEED,
                    player_position.y * -RenderingDef::BACKGROUND_TEXUTURE_1_SPEED,
                    m_backgroundSize.x, m_backgroundSize.y));
    m_backgroundSprite2.setTextureRect(
        sf::IntRect(player_position.x * -RenderingDef::BACKGROUND_TEXUTURE_2_SPEED,
                    player_position.y * -RenderingDef::BACKGROUND_TEXUTURE_2_SPEED,
                    m_backgroundSize.x, m_backgroundSize.y));
}

void BackgroundController::draw(sf::RenderTarget& target) {
    target.draw(m_boundsCircle);
    target.draw(m_backgroundSprite2);
    target.draw(m_backgroundSprite);
}
