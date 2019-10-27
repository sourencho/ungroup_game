#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include "../../../src/common/events/EventController.hpp"
#include "../../../src/common/factories/IdFactory.hpp"

struct MyListener : Catch::TestEventListenerBase {

    using TestEventListenerBase::TestEventListenerBase; // inherit constructor

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override {
        EventController::getInstance().reset();
        IdFactory::getInstance().reset();
    }

    void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override {
        // Tear-down after a test case is run
    }
};

CATCH_REGISTER_LISTENER(MyListener)