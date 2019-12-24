/**
 * Renders the background of the game.
 * Uses layered textures with parallax effects.
 */

#ifndef BackgroundController_hpp
#define BackgroundController_hpp

#include <SFML/Graphics.hpp>

#include "../../common/resources/ResourceStore.hpp"

class BackgroundController {
  public:
    explicit BackgroundController(sf::Vector2u background_size, ResourceStore& rs);
    ~BackgroundController(){};
    BackgroundController(const BackgroundController& temp_obj) = delete;
    BackgroundController& operator=(const BackgroundController& temp_obj) = delete;

    void update(sf::Vector2f player_position, sf::Vector2u window_size);
    void draw(sf::RenderTarget& target);

  private:
    ResourceStore& m_resourceStore;
    sf::Vector2u m_backgroundSize;
    sf::CircleShape m_boundsCircle;

    sf::Sprite m_backgroundSprite;  // first background layer
    sf::Sprite m_backgroundSprite2; // second background layer
};

#endif /* BackgroundController_hpp */
