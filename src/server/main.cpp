#include <iostream>
#include "systems/GameController.hpp"
#include "../common/util/game_settings.hpp"

int main(int, char const**) {
    GameController game_controller(MAX_PLAYER_COUNT, MAX_MINE_COUNT);

    while (true) {
        game_controller.step();
    }
}
