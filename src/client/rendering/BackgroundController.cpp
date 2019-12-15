#include "BackgroundController.hpp"

#include <iostream>

#include "../../common/rendering/RenderingDef.hpp"

BackgroundController::BackgroundController(sf::Vector2u background_size, ResourceStore& rs) :
    m_resourceStore(rs), m_backgroundSize(background_size) {
    auto dotted_background =
        m_resourceStore.getTexture(RenderingDef::TextureKey::dotted_background);
    m_backgroundSprite = sf::Sprite(*dotted_background);
    m_backgroundSprite2 = sf::Sprite(*dotted_background);

    m_backgroundSprite.setTextureRect(sf::IntRect(0, 0, m_backgroundSize.x, m_backgroundSize.y));
    m_backgroundSprite.setColor(sf::Color(255, 255, 255, RenderingDef::BACKGROUND_TEXTURE_1_ALPHA));

    m_backgroundSprite2.setTextureRect(sf::IntRect(dotted_background->getSize().x / 2.f,
                                                   dotted_background->getSize().y / 2.f,
                                                   m_backgroundSize.x, m_backgroundSize.y));
    m_backgroundSprite2.setColor(
        sf::Color(255, 255, 255, RenderingDef::BACKGROUND_TEXTURE_2_ALPHA));
}

void BackgroundController::update(sf::Vector2f player_position, sf::Vector2u window_size) {
    // Move texture position relative to player movement to achieve parallax effect
    m_backgroundSprite.setTextureRect(
        {static_cast<int>(player_position.x) * RenderingDef::BACKGROUND_TEXUTURE_1_SPEED,
         static_cast<int>(player_position.y) * RenderingDef::BACKGROUND_TEXUTURE_1_SPEED,
         static_cast<int>(m_backgroundSize.x), static_cast<int>(m_backgroundSize.y)});
    m_backgroundSprite2.setTextureRect(
        {static_cast<int>(player_position.x) * RenderingDef::BACKGROUND_TEXUTURE_2_SPEED,
         static_cast<int>(player_position.y) * RenderingDef::BACKGROUND_TEXUTURE_2_SPEED,
         static_cast<int>(m_backgroundSize.x), static_cast<int>(m_backgroundSize.y)});
}

void BackgroundController::draw(sf::RenderTarget& target) {
    target.draw(m_backgroundSprite2);
    target.draw(m_backgroundSprite);
}
