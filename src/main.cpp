//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Group.hpp"
#include "Player.hpp"
#include "GameController.hpp"

int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile("./resources/images/icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load a music to play
    sf::Music music;
    if (!music.openFromFile("./resources/audio/nice_music.ogg")) {
        return EXIT_FAILURE;
    }

    // Play the music
    music.play();
    
    // Window
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    window.setTitle("Ungroup");

    // Create game controller
    GameController game_controller;
    
    // Create players
    
    sf::Keyboard::Key keys[] = {sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Right, sf::Keyboard::Left};
    game_controller.createPlayer(keys);

    sf::Keyboard::Key keys2[] = {sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::D, sf::Keyboard::A};
    game_controller.createPlayer(keys2);
    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
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
            game_controller.handleEvents(event);
        }

        // Update
        game_controller.update();
        
        // Display
        window.clear(sf::Color::White);
        game_controller.draw(window);
        window.display();
    }

    return EXIT_SUCCESS;
}
