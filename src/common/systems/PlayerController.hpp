#ifndef PlayerController_hpp
#define PlayerController_hpp

#include <vector>

#include "../objects/Player.hpp"
#include "../util/game_state.hpp"


class PlayerController {
 public:
    PlayerController(std::vector<std::shared_ptr<Player>>& players);
    ~PlayerController();
    PlayerController(const PlayerController& temp_obj) = delete;
    PlayerController& operator=(const PlayerController& temp_obj) = delete;

    std::shared_ptr<Player> getPlayer(uint32_t player_id);
    uint32_t createPlayer(uint32_t client_id);
    void removePlayer(uint32_t client_id);

    void update(const ClientInputs& cis);
    void updatePostPhysics();

    void setPlayerClient(uint32_t player_id, uint32_t client_id);

 private:
    std::vector<std::shared_ptr<Player>> mPlayers;
    std::unordered_map<uint32_t, uint32_t> mClientToPlayer;
    size_t nextPlayerIndex = 0;
};

#endif /* PlayerController_hpp */