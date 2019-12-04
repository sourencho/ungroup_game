#include <iostream>

#include "../common/util/game_def.hpp"
#include "../common/util/game_settings.hpp"
#include "systems/ClientGameController.hpp"
#include <SFML/Graphics.hpp>

int main(int, char const**) {
    ClientGameController client_game_controller;
    client_game_controller.start();
    return EXIT_SUCCESS;
}
