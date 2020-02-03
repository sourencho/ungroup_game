#include "RenderingController.hpp"

#include "RenderingUtil.hpp"

RenderingController::RenderingController(sf::RenderWindow& window, GameObjectController& goc,
                                         GameObjectStore& gos, uint32_t player_id, bool headless) :
    m_window(window),
    m_uiData({}), m_gameObjectController(goc), m_resourceStore(),
    m_animationController(m_resourceStore),
    m_backgroundController(sf::Vector2u(GAME_SIZE), m_resourceStore), m_playerId(player_id),
    m_gameObjectRenderer(m_resourceStore, m_gameObjectController.getResourceController(),
                         m_gameObjectController.getGroupController(),
                         m_gameObjectController.getMineController(),
                         m_gameObjectController.getPlayerController(), m_playerId),
    m_guiController(m_window.getSize(), m_resourceStore), m_headless(headless) {
    m_window.setFramerateLimit(RenderingDef::WINDOW_FRAME_LIMIT);
    sf::Vector2f window_size = sf::Vector2f(m_window.getSize());

    // Create buffer to draw game onto. Buffer can be scaled to give pixelated effect
    if (!m_buffer.create(GAME_SIZE.x, GAME_SIZE.y)) {
        throw std::runtime_error("Failed to create buffer.");
    }
    m_buffer.setSmooth(false);

    m_bufferSprite = sf::Sprite(m_buffer.getTexture());
    m_bufferSprite.setScale(RenderingDef::GAME_SCALING_FACTOR);

    // Create views to draw GUI and player view
    m_windowView = sf::View(window_size / 2.f, window_size);
    m_playerView = sf::View({}, sf::Vector2f(m_window.getSize()));

    // Connecting text parameters
    m_connectingText.setFont(*m_resourceStore.getFont(RenderingDef::FontKey::monogram));
    m_connectingText.setCharacterSize(100.f);
    m_connectingText.setFillColor(sf::Color::White);

    // Winner text parameters
    m_winnerText.setFont(*m_resourceStore.getFont(RenderingDef::FontKey::monogram));
    m_winnerText.setCharacterSize(100.f);
    m_winnerText.setFillColor(sf::Color::White);
}

void RenderingController::preUpdate() {
    // noop
}

void RenderingController::update(sf::Int32 delta_ms) {
    if (m_headless) {
        return;
    }

    m_animationController.update(delta_ms);
}

void RenderingController::postUpdate(const UIData& ui_data) {
    if (m_headless) {
        return;
    }

    auto player_position = m_gameObjectController.getPlayerPosition(m_playerId);
    m_uiData = ui_data;

    sf::Vector2f player_view_position = RenderingUtil::mapCoordToPixelScaled(
        player_position, m_window, m_windowView, RenderingDef::GAME_SCALING_FACTOR);
    m_playerView.setCenter(player_view_position);
    m_guiController.update(m_uiData);
    m_backgroundController.update(player_position);
}

void RenderingController::draw() {
    if (m_headless) {
        return;
    }

    switch (m_uiData.game_status) {
        case GameStatus::not_started: {
            drawNotStarted();
            break;
        }
        case GameStatus::playing: {
            drawPlaying();
            break;
        }
        case GameStatus::game_over: {
            drawGameOver();
            break;
        }
    }
}

void RenderingController::drawNotStarted() {
    m_window.clear(RenderingDef::CONNECTING_SCREEN_COLOR);

    m_connectingText.setString("Connecting...");
    sf::FloatRect textRect = m_connectingText.getLocalBounds();
    m_connectingText.setOrigin(textRect.left + textRect.width / 2.0f,
                               textRect.top + textRect.height / 2.0f);
    m_connectingText.setPosition(sf::Vector2f(m_window.getSize()) / 2.f);
    m_window.draw(m_connectingText);

    m_window.display();
}

void RenderingController::drawPlaying() {
    m_window.clear(RenderingDef::OUT_OF_BOUNDS_COLOR);
    m_buffer.clear(RenderingDef::OUT_OF_BOUNDS_COLOR);

    // Draw game from player view
    m_window.setView(m_playerView);

    m_backgroundController.draw(m_buffer);
    m_gameObjectRenderer.draw(m_buffer);
    m_animationController.draw(m_buffer);
    m_window.draw(m_bufferSprite);

    // Draw GUI from window view
    m_window.setView(m_windowView);
    m_gameObjectRenderer.drawUI(m_window, m_playerView);
    m_guiController.draw(m_window);

    m_buffer.display();
    m_window.display();
}

void RenderingController::drawGameOver() {
    m_window.clear(RenderingDef::WINNER_SCREEN_COLOR);

    m_winnerText.setString("Winner: " + std::to_string(m_uiData.winner_player_id));
    sf::FloatRect textRect = m_winnerText.getLocalBounds();
    m_winnerText.setOrigin(textRect.left + textRect.width / 2.0f,
                           textRect.top + textRect.height / 2.0f);
    m_winnerText.setPosition(sf::Vector2f(m_window.getSize()) / 2.f);
    m_window.draw(m_winnerText);

    m_window.display();
}