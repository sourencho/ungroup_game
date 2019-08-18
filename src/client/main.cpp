#include <SFML/Graphics.hpp>
#include <iostream>
#include "Client.hpp"
#include "../common/game_def.hpp"
#include "../common/game_settings.hpp"

int main(int, char const**) {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y), "SFML window");

    // Window
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    window.setTitle("Ungroup");


    // Setup shaders
    sf::Shader shader;
    if (!sf::Shader::isAvailable()) {
        std::cerr << "Shaders are not available." << std::endl;
    }

    // load deafult vertex shader
    const std::string DEFAULT_VERTEX_SHADER_PATH = "shaders/vertex_shader.frag";
    if (!shader.loadFromFile(DEFAULT_VERTEX_SHADER_PATH, sf::Shader::Vertex)) {
        std::cerr << "Error: Could not load vertex shader" << std::endl;
    }

    // load default fragment shader
    const std::string DEFAULT_FRAGMENT_SHADER_PATH = "shaders/fragment_shader.frag";
    const std::string CIRCLE_GRADIENT_FRAGMENT_SHADER_PATH = "shaders/circle_gradient.frag";
    if (!shader.loadFromFile(DEFAULT_FRAGMENT_SHADER_PATH, sf::Shader::Fragment)) {
        std::cerr << "Error: Could not load fragment shader" << std::endl;
    }

    // Pass resolution to shader
    shader.setUniform("u_resolution", WINDOW_RESOLUTION);

    // Create client
    sf::Keyboard::Key keys[] = {sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Right,
        sf::Keyboard::Left, sf::Keyboard::G};
    Client client(MAX_PLAYER_COUNT, MAX_MINE_COUNT, keys);

    // Start the game loop
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape
            ) {
                window.close();
            }

            // Handle game controller events
            client.handleEvents(event);
        }

        // Update
        client.update();

        // Display
        window.clear(sf::Color::Black);
        client.draw(window, &shader, USE_SHADER);
        window.display();
    }

    return EXIT_SUCCESS;
}
