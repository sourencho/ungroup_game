#include "../common/util/game_settings.hpp"
#include "systems/ServerGameController.hpp"
#include <iostream>

int main(int, char const**) {
    ServerGameController server_game_controller;
    while (true) {
        server_game_controller.step();
    }
}
