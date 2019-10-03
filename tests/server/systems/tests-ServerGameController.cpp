#include <catch.hpp>

#include "../../../src/server/systems/ServerGameController.hpp"

SCENARIO("ServerGameController runs successfully", "[server][systems][ServerGameController]") {
    GIVEN("ServerGameController is constructed") {
        ServerGameController server_game_controller(10, 10);

        WHEN("is updated") {
            server_game_controller.update();
        }
    }
}