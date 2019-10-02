#include <iostream>
#include "systems/ServerGameController.hpp"
#include "../common/util/game_settings.hpp"

int main(int, char const**) {
    ServerGameController server_game_controller(MAX_PLAYER_COUNT, MAX_MINE_COUNT);
    while (true) {
        server_game_controller.update();
    }
}
