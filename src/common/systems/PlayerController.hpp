#ifndef PlayerController_hpp
#define PlayerController_hpp

#include <vector>

#include "../objects/Player.hpp"
#include "../util/StateDef.hpp"

class PlayerController {
  public:
    PlayerController(std::vector<std::shared_ptr<Player>>& players);
    ~PlayerController();
    PlayerController(const PlayerController& temp_obj) = delete;
    PlayerController& operator=(const PlayerController& temp_obj) = delete;

    std::shared_ptr<Player> getPlayer(uint32_t player_id);
    uint32_t createPlayer(uint32_t client_id);
    void update(std::shared_ptr<PlayerInputs> pi);
    void updatePostPhysics();

  private:
    void addEventListeners();
    void removePlayer(uint32_t player_id);
    void handleClientDisconnectedEvent(std::shared_ptr<Event> event);

    std::vector<std::shared_ptr<Player>> m_players;
    size_t nextPlayerIndex = 0;
};

#endif /* PlayerController_hpp */