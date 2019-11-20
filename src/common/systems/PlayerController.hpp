#ifndef PlayerController_hpp
#define PlayerController_hpp

#include <vector>

#include "../events/Event.hpp"
#include "../objects/Player.hpp"
#include "../util/InputDef.hpp"

class PlayerController {
  public:
    PlayerController(std::vector<std::shared_ptr<Player>>& players);
    ~PlayerController();
    PlayerController(const PlayerController& temp_obj) = delete;
    PlayerController& operator=(const PlayerController& temp_obj) = delete;

    std::shared_ptr<Player> getPlayer(uint32_t player_id);
    uint32_t createPlayer(uint32_t client_id);
    void update(const InputDef::PlayerInputs& pi);
    void updatePostPhysics();

  private:
    void addEventListeners();
    void removePlayer(uint32_t player_id);
    void handleClientDisconnectedEvent(std::shared_ptr<Event> event);

    std::vector<std::shared_ptr<Player>> m_players;
    size_t nextPlayerIndex = 0;
};

#endif /* PlayerController_hpp */