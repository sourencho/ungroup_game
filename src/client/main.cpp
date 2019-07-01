// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Client.hpp"
#include "../common/game_def.hpp"


int main(int, char const**) {
    // Create the main window
    sf::Vector2f WINDOW_RESOLUTION(800, 600);
    sf::RenderWindow window(sf::VideoMode(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y), "SFML window");

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile("../resources/images/icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load a music to play
    sf::Music music;
    if (!music.openFromFile("../resources/audio/nice_music.ogg")) {
        return EXIT_FAILURE;
    }

    // Play the music
    // music.play();

    // Window
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    window.setTitle("Ungroup");

    // Setup the shaders
    sf::Shader shader;

    if (!sf::Shader::isAvailable()) {
        std::cerr << "Shaders are not available." << std::endl;
        return EXIT_FAILURE;
    }

    // load deafult vertex shader
    const std::string DEFAULT_VERTEX_SHADER_PATH = "shaders/vertex_shader.frag";
    if (!shader.loadFromFile(DEFAULT_VERTEX_SHADER_PATH, sf::Shader::Vertex)) {
        std::cout << "Error: Could not load vertex shader" << std::endl;
    }

    // load default fragment shader
    const std::string DEFAULT_FRAGMENT_SHADER_PATH = "shaders/fragment_shader.frag";
    const std::string CIRCLE_GRADIENT_FRAGMENT_SHADER_PATH = "shaders/circle_gradient.frag";
    if (!shader.loadFromFile(DEFAULT_FRAGMENT_SHADER_PATH, sf::Shader::Fragment)) {
        std::cout << "Error: Could not load fragment shader" << std::endl;
    }

    // Pass resolution to shader
    shader.setUniform("u_resolution", WINDOW_RESOLUTION);

    // Create client
    sf::Keyboard::Key keys[] = {sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Right,
        sf::Keyboard::Left};
    Client client_1(MAX_PLAYER_COUNT, keys);

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
            client_1.handleEvents(event);
        }

        // Update
        client_1.update();

        // Display
        window.clear(sf::Color::Black);
        client_1.draw(window, &shader);
        window.display();
    }

    return EXIT_SUCCESS;
}
