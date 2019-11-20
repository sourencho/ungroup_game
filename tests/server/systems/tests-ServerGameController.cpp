#include <catch.hpp>

#include "../../../src/server/systems/ServerGameController.hpp"

SCENARIO("ServerGameController runs successfully", "[server][systems][ServerGameController]") {
    GIVEN("ServerGameController is constructed") {
        ServerGameController server_game_controller;
        WHEN("is stepped") {
            server_game_controller.step();
        }
    }
}