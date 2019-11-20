#include <catch.hpp>
#include <iostream>

#include "../../../src/client/systems/ClientGameController.hpp"

// TODO(sourenp|#109): Figure out how to test without opening a window and remove the "skip" tag.
SCENARIO("ClientGameController runs successfully",
         "[client][systems][ClientGameController][skip]") {
    GIVEN("ClientGameController is constructed") {
        sf::RenderWindow window(sf::VideoMode(), "My window");
        ClientInputKeys keys = {sf::Keyboard::Up,   sf::Keyboard::Down, sf::Keyboard::Right,
                                sf::Keyboard::Left, sf::Keyboard::G,    sf::Keyboard::Escape,
                                sf::Keyboard::Space};
        ClientGameController client_game_controller(keys, window);

        WHEN("is stepped") {
            client_game_controller.step();
        }
    }
}
