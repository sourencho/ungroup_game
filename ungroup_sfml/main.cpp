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
#include "ResourcePath.hpp"
#include <iostream>
#include "Group.hpp"
#include "Player.hpp"

int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load a music to play
    sf::Music music;
    if (!music.openFromFile(resourcePath() + "nice_music.ogg")) {
        return EXIT_FAILURE;
    }

    // Play the music
    music.play();
    
    // Window
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    window.setTitle("Ungroup");

    // Create group one
    Player player_one((sf::Keyboard::Key[]) {sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::D, sf::Keyboard::A});
    
    Player player_two((sf::Keyboard::Key[]) {sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Right, sf::Keyboard::Left});
    
    Group group_one(10.f, sf::Vector2f(10.f, 10.f), sf::Color(100, 250, 50));
    group_one.addMemeber(&player_one);
    group_one.addMemeber(&player_two);

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

            // Handle group event
            player_one.handleEvents(event);
            player_two.handleEvents(event);
            group_one.handleEvents(event);
        }

        // Update group
        player_one.update();
        player_two.update();
        group_one.update();
        
        // Clear screen
        window.clear(sf::Color::White);

        // Draw group
        group_one.draw(window);
        
        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
