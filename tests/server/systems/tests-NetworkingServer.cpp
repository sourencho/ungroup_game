#include <catch.hpp>

#include "../../../src/server/systems/NetworkingServer.hpp"

SCENARIO("NetworkingServer runs successfully", "[server, systems]") {
    GIVEN("NetworkingServer is constructed") {
        NetworkingServer networking_server;
    }
}
