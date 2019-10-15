#include <catch.hpp>
#include "../../../src/common/factories/IdFactory.hpp"


SCENARIO("Several types of ids used", "[common][systems][IdFactory]") {
    GIVEN("An id of type 0 is fetched") {
        WHEN("An id of type 0 is fetched") {
            uint32_t first_zero_id = IdFactory::getInstance().getNextId(0);
            size_t first_zero_index = IdFactory::getInstance().getIndex(first_zero_id);
            uint16_t first_zero_id_type = IdFactory::getInstance().getType(first_zero_id);
            THEN("The first id value, index and type are correct") {
                REQUIRE(first_zero_id == 0);
                REQUIRE(first_zero_index == 0);
                REQUIRE(first_zero_id_type == 0);
            }
        }
        WHEN("An id of type 0 is fetched again") {
            uint32_t second_zero_id = IdFactory::getInstance().getNextId(0);
            uint16_t second_zero_id_type = IdFactory::getInstance().getType(second_zero_id);
            size_t second_zero_index = IdFactory::getInstance().getIndex(second_zero_id);
            THEN("The second id value, index and type are correct") {
                REQUIRE(second_zero_id == 1);
                REQUIRE(second_zero_index == 1);
                REQUIRE(second_zero_id_type == 0);
            }
        }
        WHEN("An id of type 1 is fetched") {
            uint32_t first_one_id = IdFactory::getInstance().getNextId(1);
            size_t first_one_index = IdFactory::getInstance().getIndex(first_one_id);
            uint16_t first_one_id_type = IdFactory::getInstance().getType(first_one_id);
            THEN("The id value, index and type are correct") {
                REQUIRE(first_one_id == OFFSET_VALUE);
                REQUIRE(first_one_index == 0);
                REQUIRE(first_one_id_type == 1);
            }
        }
    }
}

