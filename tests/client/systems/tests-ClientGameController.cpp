#include <catch.hpp>
#include <iostream>

#include "../../../src/client/systems/ClientGameController.hpp"

// TODO(sourenp|#109): Figure out how to test without opening a window and remove the "skip" tag.
SCENARIO("ClientGameController runs successfully",
         "[client][systems][ClientGameController][skip]") {
    GIVEN("ClientGameController is constructed") {
        ClientGameController client_game_controller;

        WHEN("is stepped") {
            client_game_controller.step();
        }
    }
}
