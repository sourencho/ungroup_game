#include <iostream>
#include "GameController.hpp"

int main(int, char const**) {
    GameController game_controller(MAX_PLAYER_COUNT);

    while (true) {
        game_controller.update();
    }
}
