add_executable(ug-test
    tests-main.cpp
    common/objects/tests-CircleRigidBody.cpp
    common/events/tests-EventController.cpp
    common/factories/tests-IdFactory.cpp
    server/systems/tests-ServerGameController.cpp
    client/systems/tests-ClientGameController.cpp
    client/rendering/tests-AnimatedSprite.cpp
    client/rendering/tests-AnimationController.cpp
)

target_link_libraries(ug-test PRIVATE server-lib common-lib client-lib catch)
