#include "../common/util/game_settings.hpp"
#include "systems/ServerGameController.hpp"
#include <iostream>

int main(int, char const**) {
    ServerGameController server_game_controller;
    server_game_controller.start();
    return EXIT_SUCCESS;
}
