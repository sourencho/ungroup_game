/**
 * Renders game objects.
 */

#ifndef GameObjectRenderer_hpp
#define GameObjectRenderer_hpp

#include <SFML/Graphics.hpp>

#include "../../common/systems/GroupController.hpp"
#include "../../common/systems/MineController.hpp"
#include "../../common/systems/ResourceController.hpp"
#include "DrawableGroup.hpp"
#include "DrawableMine.hpp"

class GameObjectRenderer {
  public:
    explicit GameObjectRenderer(ResourceStore& rs, ResourceController& rc, GroupController& gc,
                                MineController& mc);
    ~GameObjectRenderer(){};
    GameObjectRenderer(const GameObjectRenderer& temp_obj) = delete;
    GameObjectRenderer& operator=(const GameObjectRenderer& temp_obj) = delete;

    /**
     * Draw the game object's onto the scalable buffer;
     */
    void draw(sf::RenderTarget& target);

    /**
     * Draw the game object specific UI onto the unscaled window.
     * This is called after draw, so the UI is drawn over the game objects.
     */
    void drawUI(sf::RenderWindow& window, sf::View& player_view);

  private:
    void drawGroups(sf::RenderTarget& target);
    void drawMines(sf::RenderTarget& target);
    void drawGroupPlayerIds(sf::RenderWindow& window, sf::View& player_view);
    void drawMineDirections(sf::RenderWindow& window, sf::View& player_view);

    std::vector<std::unique_ptr<DrawableGroup>> m_drawableGroups;
    std::vector<std::unique_ptr<DrawableMine>> m_drawableMines;
    std::vector<sf::Text> m_groupPlayerTexts;
    std::vector<sf::RectangleShape> m_mineDirections;

    MineController& m_mineController;
    GroupController& m_groupController;
    ResourceController& m_resourceController;
};

#endif /* GameObjectRenderer_hpp */
