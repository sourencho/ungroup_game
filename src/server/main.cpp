#include <iostream>
#include "GameController.hpp"

int main(int, char const**) {
    // Create game controller
    GameController game_controller(MAX_PLAYER_COUNT);

    // Create players
    //size_t player_1_id = game_controller.createPlayer();

    while (true) {
        // Update game state based on client inputs
        // game_controller.update(client_1.getId(), client_1.getDirection());
    }
}
