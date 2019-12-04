#include "RenderingController.hpp"

RenderingController::RenderingController(sf::RenderWindow& window, GameObjectController& goc,
                                         ResourceStore& rs) :
    m_window(window),
    m_gameObjectController(goc), m_resourceStore(rs), m_animationController(m_resourceStore),
    m_guiController(m_window.getSize(), m_resourceStore) {
    sf::Vector2f window_size = sf::Vector2f(m_window.getSize());

    // Create buffer to draw game onto. Buffer can be scaled to give pixelated effect
    if (!m_buffer.create(window_size.x, window_size.y)) {
        throw std::runtime_error("Failed to create buffer.");
    }
    m_buffer.setSmooth(false);

    m_bufferSprite = sf::Sprite(m_buffer.getTexture());
    m_bufferScalingFactor = {GAME_SCALE * GAME_SIZE.x / WINDOW_RESOLUTION.x,
                             GAME_SCALE * GAME_SIZE.y / WINDOW_RESOLUTION.y};
    m_bufferSprite.setScale(m_bufferScalingFactor);

    // Create views to draw GUI and player view
    m_windowView = sf::View(window_size / 2.f, window_size);
    m_playerView = sf::View({}, sf::Vector2f(m_window.getSize()));
}

void RenderingController::preUpdate() {
    // noop
}

void RenderingController::update(sf::Int32 delta_ms) {
    m_animationController.update(delta_ms);
}

void RenderingController::postUpdate(sf::Vector2f player_position, UIData ui_data) {
    sf::Vector2f player_view_position = {player_position.x * m_bufferScalingFactor.x,
                                         player_position.y * m_bufferScalingFactor.y};
    m_playerView.setCenter(player_view_position);
    m_guiController.update(ui_data);
}

void RenderingController::draw() {
    m_window.clear(sf::Color::Green);
    m_buffer.clear(BACKGROUND_COLOR);

    // Draw game from player view
    m_window.setView(m_playerView);
    m_gameObjectController.draw(m_buffer);
    m_animationController.draw(m_buffer);
    m_window.draw(m_bufferSprite);

    // Draw GUI from window view
    m_window.setView(m_windowView);
    m_guiController.draw(m_window);

    m_buffer.display();
    m_window.display();
}