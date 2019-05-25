// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Group.hpp"
#include "Player.hpp"
#include "Client.hpp"
#include "GameController.hpp"
#include "game_def.hpp"


int main(int, char const**)
{
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
    music.play();

    // Window
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    window.setTitle("Ungroup");

    // Create game controller
    GameController game_controller(MAX_PLAYER_COUNT);

    // Create client
    sf::Keyboard::Key keys[] = {sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Right, sf::Keyboard::Left};
    Client client_1(MAX_PLAYER_COUNT, keys);

    // Create players
    size_t player_1_id = game_controller.createPlayer();
    client_1.setId(player_1_id);

    while (true) {};

    /*
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
            client_1.handleEvents(event);
        }

        // Update
        game_controller.update(client_1.getId(), client_1.getDirection());
        client_1.update(game_controller.getActiveGroups());

        // Display
        window.clear(sf::Color::White);
        client_1.draw(window);
        window.display();
    }
    */

    return EXIT_SUCCESS;
}
