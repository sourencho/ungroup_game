#include <catch.hpp>

#include "../../../src/client/systems/NetworkingClient.hpp"

SCENARIO("NetworkingClient runs successfully", "[client, systems]") {
    GIVEN("NetworkingClient is constructed") {
        NetworkingClient networking_client;
    }
}
