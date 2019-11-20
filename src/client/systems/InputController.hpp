#ifndef InputController_hpp
#define InputController_hpp

#include "../../common/util/InputDef.hpp"
#include <SFML/Graphics.hpp>

class InputController {
  public:
    explicit InputController(InputDef::InputKeys keys);
    ~InputController(){};
    InputController(const InputController& temp_obj) = delete;
    InputController& operator=(const InputController& temp_obj) = delete;

    std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
    collectInputs(sf::RenderWindow& window);
    InputDef::PlayerInputs getPlayerInputs(uint32_t player_id);

  private:
    InputDef::InputKeys m_inputKeys;

    InputDef::ReliableInput m_reliableInput;
    InputDef::UnreliableInput m_unreliableInput;

    InputDef::PlayerInputs m_playerInputs; // Cache
};

#endif /* InputController_hpp */
