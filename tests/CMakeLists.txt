add_executable(ug-test
    tests-main.cpp
    common/objects/tests-CircleRigidBody.cpp
    common/events/tests-EventController.cpp
    common/systems/tests-IdController.cpp
    server/systems/tests-ServerGameController.cpp
    client/systems/tests-ClientGameController.cpp
)

target_link_libraries(ug-test PRIVATE server-lib common-lib client-lib catch)
