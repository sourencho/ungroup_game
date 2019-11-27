add_executable(ug-test
    tests-main.cpp
    common/events/tests-EventController.cpp
    common/factories/tests-IdFactory.cpp
    common/physics/tests-CollisionUtil.cpp
    common/objects/tests-CircleRigidBody.cpp
    server/systems/tests-ServerGameController.cpp
    client/systems/tests-ClientGameController.cpp
    client/rendering/tests-AnimatedSprite.cpp
    client/rendering/tests-AnimationController.cpp
    common/systems/tests-ResourceController.cpp
)

target_link_libraries(ug-test PRIVATE server-lib common-lib client-lib catch)
