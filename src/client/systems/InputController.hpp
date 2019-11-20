#ifndef InputController_hpp
#define InputController_hpp

#include "../../common/util/StateDef.hpp"
#include <SFML/Graphics.hpp>

struct ClientInputKeys {
    sf::Keyboard::Key up, down, right, left, joinable, ungroup, stop;
};

class InputController {
  public:
    explicit InputController(ClientInputKeys keys);
    ~InputController(){};
    InputController(const InputController& temp_obj) = delete;
    InputController& operator=(const InputController& temp_obj) = delete;

    std::pair<ClientReliableUpdate, ClientUnreliableUpdate> collectInputs(sf::RenderWindow& window);
    PlayerInputs getPlayerInputs(uint32_t player_id);

  private:
    ClientInputKeys m_clientInputKeys;

    ClientReliableUpdate m_clientReliableUpdate;
    ClientUnreliableUpdate m_clientUnreliableUpdate;

    PlayerInputs m_playerInputs; // Cache
};

#endif /* InputController_hpp */
