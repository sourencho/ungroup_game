#include <catch.hpp>
#include <iostream>

#include "../../../src/client/systems/ClientGameController.hpp"

SCENARIO("ClientGameController runs successfully", "[client][systems][ClientGameController]") {
    GIVEN("ClientGameController is constructed") {
        sf::RenderWindow window(sf::VideoMode(), "My window");
        ClientInputKeys keys = {sf::Keyboard::Up,   sf::Keyboard::Down, sf::Keyboard::Right,
                                sf::Keyboard::Left, sf::Keyboard::G,    sf::Keyboard::Escape,
                                sf::Keyboard::Space};
        ClientGameController client_game_controller(10, 10, keys, window);

        WHEN("is stepped") {
            client_game_controller.step();
        }
    }
}
