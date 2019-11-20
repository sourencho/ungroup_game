#include "InputController.hpp"

InputController::InputController(ClientInputKeys keys) : m_clientInputKeys(keys) {
}

std::pair<ClientReliableUpdate, ClientUnreliableUpdate>
InputController::collectInputs(sf::RenderWindow& window) {
    // Reset updates
    m_clientReliableUpdate.setAll(false);
    m_clientUnreliableUpdate.setAll(false);

    // Process events
    sf::Event event;
    while (window.pollEvent(event)) {
        // Close window: exit
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            m_clientReliableUpdate = {
                .toggle_ungroup = sf::Keyboard::isKeyPressed(m_clientInputKeys.ungroup),
                .toggle_joinable = sf::Keyboard::isKeyPressed(m_clientInputKeys.joinable),
            };
            m_clientUnreliableUpdate = {
                .toggle_up = sf::Keyboard::isKeyPressed(m_clientInputKeys.up),
                .toggle_down = sf::Keyboard::isKeyPressed(m_clientInputKeys.down),
                .toggle_right = sf::Keyboard::isKeyPressed(m_clientInputKeys.right),
                .toggle_left = sf::Keyboard::isKeyPressed(m_clientInputKeys.left),
                .toggle_stop = sf::Keyboard::isKeyPressed(m_clientInputKeys.stop),
            };
        }
    }

    return std::pair<ClientReliableUpdate, ClientUnreliableUpdate>(m_clientReliableUpdate,
                                                                   m_clientUnreliableUpdate);
}

PlayerInputs InputController::getPlayerInputs(uint32_t player_id) {
    m_playerInputs.player_reliable_updates.clear();
    m_playerInputs.player_unreliable_updates.clear();

    if (!m_clientReliableUpdate.allFalse()) {
        PlayerReliableUpdate player_reliable_update = {
            .player_id = player_id,
            .client_reliable_update = m_clientReliableUpdate,
        };
        m_playerInputs.player_reliable_updates.push_back(player_reliable_update);
    }
    if (!m_clientUnreliableUpdate.allFalse()) {
        PlayerUnreliableUpdate player_unreliable_update = {
            .player_id = player_id,
            .client_unreliable_update = m_clientUnreliableUpdate,
        };
        m_playerInputs.player_unreliable_updates.push_back(player_unreliable_update);
    }

    return m_playerInputs;
}