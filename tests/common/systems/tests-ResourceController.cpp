#include <catch.hpp>
#include <iostream>

#include "../../../src/common/systems/ResourceController.hpp"

SCENARIO("Resource is added, subtracted and moved", "[common][systems][ResourceController]") {
    GIVEN("ResourceController is created") {
        ResourceController resource_controller;
        uint32_t id = 1;
        uint32_t from = 1;
        uint32_t to = 2;
        WHEN("For id 1: set RED to 10") {
            resource_controller.set(id, ResourceType::RED, 10);
            THEN("Resource counts for id 1 are correct") {
                std::array<uint32_t, RESOURCE_TYPE_COUNT> expected_counts = {10, 0, 0, 0};
                REQUIRE(resource_controller.get(id) == expected_counts);
                REQUIRE(resource_controller.get(id, ResourceType::RED) == 10);
            }
        }
        WHEN("For id 1: Set RED to 10 and add 4") {
            uint32_t add_count, new_count;
            resource_controller.set(id, ResourceType::RED, 10);
            std::tie(add_count, new_count) = resource_controller.add(id, ResourceType::RED, 4);
            THEN("Operation counts and resource counts for id 1 are correct") {
                std::array<uint32_t, RESOURCE_TYPE_COUNT> expected_counts = {14, 0, 0, 0};
                REQUIRE(resource_controller.get(id) == expected_counts);
                REQUIRE(add_count == 4);
                REQUIRE(new_count == 14);
            }
        }
        WHEN("For id 1: Set RED to 10 and subtract 4") {
            uint32_t subtract_count, new_count;
            resource_controller.set(id, ResourceType::RED, 10);
            std::tie(subtract_count, new_count) =
                resource_controller.subtract(id, ResourceType::RED, 4);
            THEN("Operation counts and resource counts for id 1 are correct") {
                std::array<uint32_t, RESOURCE_TYPE_COUNT> expected_counts = {6, 0, 0, 0};
                REQUIRE(resource_controller.get(id) == expected_counts);
                REQUIRE(subtract_count == 4);
                REQUIRE(new_count == 6);
            }
        }
        WHEN("For id 1: Set RED to 10 and add max") {
            uint32_t add_count, new_count;
            resource_controller.set(id, ResourceType::RED, 10);
            std::tie(add_count, new_count) = resource_controller.add(
                id, ResourceType::RED, std::numeric_limits<uint32_t>::max());
            THEN("Operation counts and resource counts for id 1 are correct") {
                std::array<uint32_t, RESOURCE_TYPE_COUNT> expected_counts = {
                    std::numeric_limits<uint32_t>::max(), 0, 0, 0};
                REQUIRE(resource_controller.get(id) == expected_counts);
                REQUIRE(add_count == std::numeric_limits<uint32_t>::max() - 10);
                REQUIRE(new_count == std::numeric_limits<uint32_t>::max());
            }
        }
        WHEN("For id 1: Set RED to 10 and subtract 20") {
            uint32_t subtract_count, new_count;
            resource_controller.set(id, ResourceType::RED, 10);
            std::tie(subtract_count, new_count) =
                resource_controller.subtract(id, ResourceType::RED, 20);
            THEN("Operation counts and resource counts for id 1 are correct") {
                std::array<uint32_t, RESOURCE_TYPE_COUNT> expected_counts = {0, 0, 0, 0};
                REQUIRE(resource_controller.get(id) == expected_counts);
                REQUIRE(subtract_count == 10);
                REQUIRE(new_count == 0);
            }
        }
        WHEN("For id 1: Set RED to 10 and move 4 to 2") {
            uint32_t subtract_count, sender_count, add_count, reciever_count;
            resource_controller.set(from, ResourceType::RED, 10);
            std::tie(subtract_count, sender_count, add_count, reciever_count) =
                resource_controller.move(from, to, ResourceType::RED, 4);
            THEN("Operation counts and resource counts for id 1 are correct") {
                std::array<uint32_t, RESOURCE_TYPE_COUNT> expected_counts_for_from = {6, 0, 0, 0};
                std::array<uint32_t, RESOURCE_TYPE_COUNT> expected_counts_for_to = {4, 0, 0, 0};
                REQUIRE(resource_controller.get(from) == expected_counts_for_from);
                REQUIRE(resource_controller.get(to) == expected_counts_for_to);
                REQUIRE(subtract_count == 4);
                REQUIRE(sender_count == 6);
                REQUIRE(add_count == 4);
                REQUIRE(reciever_count == 4);
            }
        }
    }
}