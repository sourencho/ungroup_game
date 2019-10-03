#include <catch.hpp>

#include "../../../src/client/systems/ClientGameController.hpp"

SCENARIO("ClientGameController runs successfully", "[client, systems]") {
    GIVEN("ClientGameController is constructed") {
        sf::Keyboard::Key keys[] = {sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Right,
            sf::Keyboard::Left, sf::Keyboard::G};
        ClientGameController client_game_controller(10, 10, keys);

        WHEN("is updated") {
            client_game_controller.update();
        }
    }
}
