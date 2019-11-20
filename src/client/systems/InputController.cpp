#include "InputController.hpp"

InputController::InputController(InputDef::InputKeys keys) :
    m_inputKeys(keys), m_reliableInput{false, false}, m_unreliableInput{false, false, false, false,
                                                                        false} {
}

std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>
InputController::collectInputs(sf::RenderWindow& window) {
    // Reset updates
    m_reliableInput.setAll(false);
    m_unreliableInput.setAll(false);

    // Process events
    sf::Event event;
    while (window.pollEvent(event)) {
        // Close window: exit
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            m_reliableInput = {
                .toggle_ungroup = sf::Keyboard::isKeyPressed(m_inputKeys.ungroup),
                .toggle_joinable = sf::Keyboard::isKeyPressed(m_inputKeys.joinable),
            };
            m_unreliableInput = {
                .toggle_up = sf::Keyboard::isKeyPressed(m_inputKeys.up),
                .toggle_down = sf::Keyboard::isKeyPressed(m_inputKeys.down),
                .toggle_right = sf::Keyboard::isKeyPressed(m_inputKeys.right),
                .toggle_left = sf::Keyboard::isKeyPressed(m_inputKeys.left),
                .toggle_stop = sf::Keyboard::isKeyPressed(m_inputKeys.stop),
            };
        }
    }

    return std::pair<InputDef::ReliableInput, InputDef::UnreliableInput>(m_reliableInput,
                                                                         m_unreliableInput);
}

InputDef::PlayerInputs InputController::getPlayerInputs(uint32_t player_id) {
    m_playerInputs.player_reliable_inputs.clear();
    m_playerInputs.player_unreliable_inputs.clear();

    if (!m_reliableInput.allFalse()) {
        InputDef::PlayerReliableInput player_reliable_input = {
            .player_id = player_id,
            .reliable_input = m_reliableInput,
        };
        m_playerInputs.player_reliable_inputs.push_back(player_reliable_input);
    }
    if (!m_unreliableInput.allFalse()) {
        InputDef::PlayerUnreliableInput player_unreliable_input = {
            .player_id = player_id,
            .unreliable_input = m_unreliableInput,
        };
        m_playerInputs.player_unreliable_inputs.push_back(player_unreliable_input);
    }

    return m_playerInputs;
}