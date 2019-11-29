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
    InputDef::InputKeys input_keys = {
        .up = sf::Keyboard::Up,
        .down = sf::Keyboard::Down,
        .right = sf::Keyboard::Right,
        .left = sf::Keyboard::Left,
        .joinable = sf::Keyboard::G,
        .ungroup = sf::Keyboard::Escape,
        .stop = sf::Keyboard::Space,
    };
    ClientGameController client_game_controller(input_keys, window, buffer, buffer_scaling_factor,
                                                buffer_sprite);

    client_game_controller.start();

    return EXIT_SUCCESS;
}
