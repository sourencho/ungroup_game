#ifndef GameObjectController_hpp
#define GameObjectController_hpp

#include <memory>

#include "../physics/PhysicsController.hpp"
#include "../util/StateDef.hpp"
#include "GameObjectStore.hpp"
#include "GroupController.hpp"
#include "MineController.hpp"
#include "PlayerController.hpp"

class GameObjectController {
  public:
    GameObjectController(PhysicsController& physics_controller, ResourceStore& resource_store);
    ~GameObjectController();

    void update(std::shared_ptr<PlayerInputs> pi);
    void updatePostPhysics();
    uint32_t createPlayerWithGroup(uint32_t client_id);
    GameState getGameState(uint32_t tick);
    void applyGameState(GameState game_state);
    void draw(sf::RenderTexture& buffer);
    sf::Vector2f getPlayerPosition(uint32_t player_id);

  private:
    void addEventListeners();
    void handleClientConnectedEvent(std::shared_ptr<Event> event);

    GameObjectStore m_gameObjectStore;
    GroupController m_groupController;
    PlayerController m_playerController;
    MineController m_mineController;
};

#endif /* GameObjectController_hpp */