#include <catch.hpp>
#include <iostream>

#include "../../../src/client/systems/ClientGameController.hpp"

SCENARIO("ClientGameController runs successfully", "[client][systems][ClientGameController]") {
    GIVEN("ClientGameController is constructed") {
        ClientInputKeys keys = {sf::Keyboard::Up,   sf::Keyboard::Down, sf::Keyboard::Right,
                                sf::Keyboard::Left, sf::Keyboard::G,    sf::Keyboard::Escape,
                                sf::Keyboard::Space};
        ClientGameController client_game_controller(10, 10, keys);

        WHEN("is updated") { client_game_controller.update(); }
    }
}
