#include <iostream>
#include "GameController.hpp"

int main(int, char const**) {
    // Create game controller
    GameController game_controller(MAX_PLAYER_COUNT);

    // Create players
    //size_t player_1_id = game_controller.createPlayer();

    while (true) {
        // TODO: What should this amount be set to?
        sf::sleep(sf::milliseconds(50));
        game_controller.update();
    }
}
