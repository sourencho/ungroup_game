#include <iostream>

#include "../common/util/game_def.hpp"
#include "../common/util/game_settings.hpp"
#include "systems/ClientGameController.hpp"
#include <SFML/Graphics.hpp>

int main(int, char const**) {
    // Create the main window
    sf::ContextSettings contSettings{};
    contSettings.antialiasingLevel = 0;
    sf::RenderWindow window(sf::VideoMode(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y), "Ungroup",
                            sf::Style::Close, contSettings);
    window.setFramerateLimit(60);

    // Create view to set the perspective of the player
    sf::View view({0.f, 0.f}, WINDOW_RESOLUTION);
    window.setView(view);

    // Create buffer to draw game onto. Buffer can be scaled to give pixelated effect
    sf::RenderTexture buffer;
    if (!buffer.create(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y))
        return -1;
    buffer.setSmooth(false);
    sf::Sprite buffer_sprite(buffer.getTexture());
    sf::Vector2f buffer_scaling_factor = {GAME_SCALE * GAME_SIZE.x / WINDOW_RESOLUTION.x,
                                          GAME_SCALE * GAME_SIZE.y / WINDOW_RESOLUTION.y};
    buffer_sprite.setScale(buffer_scaling_factor);

    // Create client game controller
    ClientInputKeys client_input_keys = {
        .up = sf::Keyboard::Up,
        .down = sf::Keyboard::Down,
        .right = sf::Keyboard::Right,
        .left = sf::Keyboard::Left,
        .joinable = sf::Keyboard::G,
        .ungroup = sf::Keyboard::Escape,
        .stop = sf::Keyboard::Space,
    };
    ClientGameController client_game_controller(client_input_keys, window);

    // Start the game loop
    while (window.isOpen()) {
        // Update
        client_game_controller.step();

        // Display
        window.clear(sf::Color::White);
        buffer.clear(sf::Color(12, 30, 39));

        client_game_controller.updateView(window, buffer_scaling_factor);
        client_game_controller.draw(buffer);
        buffer.display();
        window.draw(buffer_sprite);
        window.display();
    }

    return EXIT_SUCCESS;
}
