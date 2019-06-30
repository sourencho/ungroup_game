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
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

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

    if (!sf::Shader::isAvailable())
    {
        std::cerr << "Shaders are not available." << std::endl;
        return EXIT_FAILURE;
    }

    // load only the vertex shader
    if (!shader.loadFromFile("shaders/vertex_shader.vert", sf::Shader::Vertex))
    {
        // error...
        std::cout << "Error: Could not load vertex shader" << std::endl;
    }

    // load only the fragment shader
    if (!shader.loadFromFile("shaders/fragment_shader.frag", sf::Shader::Fragment))
    {
        // error...
        std::cout << "Error: Could not load fragment shader" << std::endl;
    }

    // shader.setUniform("texture", sf::Shader::CurrentTexture);

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
