#include <catch.hpp>
#include "../../../src/common/systems/IdController.hpp"


SCENARIO("Several types of ids used", "[common][systems][IdController]") {
    GIVEN("An id of type 0 is fetched") {
        WHEN("An id of type 0 is fetched") {
            uint32_t first_zero_id = IdController::getInstance().getNextId(0);
            size_t first_zero_index = IdController::getInstance().getIndex(first_zero_id);
            uint16_t first_zero_id_type = IdController::getInstance().getType(first_zero_id);
            THEN("The first id value, index and type are correct") {
                REQUIRE(first_zero_id == 0);
                REQUIRE(first_zero_index == 0);
                REQUIRE(first_zero_id_type == 0);
            }
        }
        WHEN("An id of type 0 is fetched again") {
            uint32_t second_zero_id = IdController::getInstance().getNextId(0);
            uint16_t second_zero_id_type = IdController::getInstance().getType(second_zero_id);
            size_t second_zero_index = IdController::getInstance().getIndex(second_zero_id);
            THEN("The second id value, index and type are correct") {
                REQUIRE(second_zero_id == 1);
                REQUIRE(second_zero_index == 1);
                REQUIRE(second_zero_id_type == 0);
            }
        }
        WHEN("An id of type 1 is fetched") {
            uint32_t first_one_id = IdController::getInstance().getNextId(1);
            size_t first_one_index = IdController::getInstance().getIndex(first_one_id);
            uint16_t first_one_id_type = IdController::getInstance().getType(first_one_id);
            THEN("The id value, index and type are correct") {
                REQUIRE(first_one_id == OFFSET_VALUE);
                REQUIRE(first_one_index == 0);
                REQUIRE(first_one_id_type == 1);
            }
        }
    }
}

