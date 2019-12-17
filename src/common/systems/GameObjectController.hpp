#ifndef GameObjectController_hpp
#define GameObjectController_hpp

#include <memory>
#include <unordered_map>

#include "../physics/PhysicsController.hpp"
#include "../util/StateDef.hpp"
#include "../util/game_def.hpp"
#include "GameObjectStore.hpp"
#include "GroupController.hpp"
#include "MineController.hpp"
#include "PlayerController.hpp"
#include "ResourceController.hpp"
#include "../bots/Bot.hpp"

class GameObjectController {
  public:
    GameObjectController(PhysicsController& physics_controller, ResourceStore& resource_store);
    ~GameObjectController();

    void update(const InputDef::PlayerInputs& pi);
    void updatePostPhysics();
    uint32_t createPlayerWithGroup(uint32_t client_id);
    GameStateObject getGameStateObject();
    void applyGameStateObject(GameStateObject game_state_object);

    /**
     * Draw the game object's onto the scalable buffer;
     */
    void draw(sf::RenderTexture& buffer);

    /**
     * Draw the game object specific UI onto the unscaled window.
     * This is called after draw, so the UI is drawn over the game objects.
     */
    void drawUI(sf::RenderWindow& window, sf::View& player_view);

    sf::Vector2f getPlayerPosition(uint32_t player_id);
    std::array<uint32_t, RESOURCE_TYPE_COUNT> getPlayerResources(uint32_t player_id);
    std::pair<bool, uint32_t> getGameOver();

    // bot methods
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> getBotMove(uint32_t bot_player_id, BotStrategy strategy);

  private:
    void addEventListeners();
    void handleClientConnectedEvent(std::shared_ptr<Event> event);
    void handleCollisionEvent(std::shared_ptr<Event> event);
    void transferResources(uint32_t circle_a_id, uint32_t circle_b_id);

    ResourceController m_resourceController;
    GameObjectStore m_gameObjectStore;
    PlayerController m_playerController;
    GroupController m_groupController;
    MineController m_mineController;

    Bot m_bot;
};

#endif /* GameObjectController_hpp */
