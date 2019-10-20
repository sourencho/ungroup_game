#include <iostream>

#include "../common/util/game_def.hpp"
#include "../common/util/game_settings.hpp"
#include "systems/ClientGameController.hpp"
#include <SFML/Graphics.hpp>

int main(int, char const **) {
    // Create the main window
    sf::RenderWindow window(
        sf::VideoMode(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y), "SFML window");

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
    const std::string DEFAULT_VERTEX_SHADER_PATH =
        "src/shaders/vertex_shader.vert";
    if (!shader.loadFromFile(DEFAULT_VERTEX_SHADER_PATH, sf::Shader::Vertex)) {
        std::cerr << "Error: Could not load vertex shader" << std::endl;
    }

    // load default fragment shader
    const std::string DEFAULT_FRAGMENT_SHADER_PATH = "src/shaders/fragment_shader.frag";
    //const std::string CIRCLE_GRADIENT_FRAGMENT_SHADER_PATH = "src/shaders/circle_gradient.frag";
    //const std::string RAND_FRAGMENT_SHADER_PATH = "src/shaders/random.frag";
    if (!shader.loadFromFile(DEFAULT_FRAGMENT_SHADER_PATH,
                             sf::Shader::Fragment)) {
        std::cerr << "Error: Could not load fragment shader" << std::endl;
    }

    // Pass resolution to shader
    if (USE_SHADER) {
        shader.setUniform("u_resolution", WINDOW_RESOLUTION);
    }

    // Create client
    Keys keys = {sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Right, sf::Keyboard::Left,
        sf::Keyboard::G};
    ClientGameController client_game_controller(MAX_PLAYER_COUNT, MAX_MINE_COUNT, keys);

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
                event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            // Handle game controller events
            client_game_controller.handleEvents(event);
        }

        // Update
        client_game_controller.update();

        // Display
        window.clear(sf::Color::Black);
        client_game_controller.draw(window, &shader, USE_SHADER);
        window.display();
    }

    return EXIT_SUCCESS;
}
