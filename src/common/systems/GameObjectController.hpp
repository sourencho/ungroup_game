#ifndef GameObjectController_hpp
#define GameObjectController_hpp

#include <memory>
#include <unordered_map>

#include "../bots/Bot.hpp"
#include "../physics/PhysicsController.hpp"
#include "../util/StateDef.hpp"
#include "../util/game_def.hpp"
#include "GameObjectStore.hpp"
#include "GroupController.hpp"
#include "LevelController.hpp"
#include "MineController.hpp"
#include "PlayerController.hpp"
#include "ResourceController.hpp"

class GameObjectController {
  public:
    GameObjectController(GameObjectStore& gos, LevelKey level_key);
    ~GameObjectController();

    void update(const InputDef::PlayerInputs& pi);
    uint32_t createPlayerWithGroup(uint32_t client_id);
    GameStateObject getGameStateObject();
    void applyGameStateObject(GameStateObject game_state_object);

    sf::Vector2f getPlayerPosition(uint32_t player_id);
    std::array<uint32_t, RESOURCE_TYPE_COUNT> getPlayerResources(uint32_t player_id);
    std::pair<bool, uint32_t> getGameOver();

    GroupController& getGroupController();
    MineController& getMineController();
    ResourceController& getResourceController();
    PlayerController& getPlayerController();

    // bot methods
    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput> getBotMove(uint32_t bot_player_id,
                                                                             BotStrategy strategy);

  private:
    void addEventListeners();
    void handleClientConnectedEvent(std::shared_ptr<Event> event);
    void handleCollisionEvent(std::shared_ptr<Event> event);
    void transferResources(uint32_t circle_a_id, uint32_t circle_b_id);

    ResourceController m_resourceController;
    GameObjectStore& m_gameObjectStore;
    PlayerController m_playerController;
    GroupController m_groupController;
    MineController m_mineController;
    LevelController m_levelController;

    Bot m_bot;
};

#endif /* GameObjectController_hpp */
