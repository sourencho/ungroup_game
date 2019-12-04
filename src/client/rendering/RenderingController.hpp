
/**
 * Renders game.
 * Manages animations and ui.
 */

#ifndef RenderingController_hpp
#define RenderingController_hpp

#include <SFML/Graphics.hpp>

#include "../../common/systems/GameObjectController.hpp"
#include "../../common/util/game_settings.hpp"
#include "../rendering/GUIController.hpp"
#include "AnimationController.hpp"

class RenderingController {
  public:
    RenderingController(sf::RenderWindow& window, GameObjectController& goc, ResourceStore& rs);
    ~RenderingController(){};

    void draw();

    void preUpdate();
    void update(sf::Int32 delta_ms);
    void postUpdate(sf::Vector2f player_position, UIData ui_data);

  private:
    sf::RenderWindow& m_window;
    sf::View m_playerView; // View specific to player
    sf::View m_windowView; // View of window. Used to draw UI elements independent of player view.
    sf::Vector2f m_bufferScalingFactor; // Amount by which game is scaled.
    sf::RenderTexture m_buffer;         // Buffer onto which game is drawn offscreen.
    sf::Sprite m_bufferSprite;          // Sprite used to draw m_buffer.

    GameObjectController& m_gameObjectController;
    ResourceStore& m_resourceStore;
    GUIController m_guiController;
    AnimationController m_animationController;
};

#endif /* RenderingController_hpp */