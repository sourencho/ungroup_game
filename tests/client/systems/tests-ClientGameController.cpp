#include <catch.hpp>
#include <iostream>

#include "../../../src/client/systems/ClientGameController.hpp"

// TODO(sourenp|#109): Figure out how to test without opening a window and remove the "skip" tag.
SCENARIO("ClientGameController runs successfully",
         "[client][systems][ClientGameController][skip]") {
    GIVEN("ClientGameController is constructed") {
        sf::RenderWindow window(sf::VideoMode(), "My window");
        InputDef::InputKeys keys = {sf::Keyboard::Up,   sf::Keyboard::Down, sf::Keyboard::Right,
                                    sf::Keyboard::Left, sf::Keyboard::G,    sf::Keyboard::Escape,
                                    sf::Keyboard::Space};

        sf::RenderTexture buffer;
        buffer.create(1, 1);
        sf::Vector2f buffer_scaling_factor(1.f, 1.f);
        sf::Sprite buffer_sprite(buffer.getTexture());
        ClientGameController client_game_controller(keys, window, buffer, buffer_scaling_factor,
                                                    buffer_sprite);

        WHEN("is stepped") {
            client_game_controller.step();
        }
    }
}
