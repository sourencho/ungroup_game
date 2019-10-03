add_library(common-lib
    util/util.cpp
    util/network_util.cpp
    util/game_state.cpp
    objects/CircleGameObject.cpp
    objects/Player.cpp
    objects/Mine.cpp
    objects/Circle.cpp
    objects/CircleRigidBody.cpp
    objects/Group.cpp
    events/ClientConnectedEvent.cpp
    events/Event.cpp
    events/ClientDisconnectedEvent.cpp
    events/EventController.cpp
    systems/PhysicsController.cpp
    systems/GameController.cpp
    systems/LevelController.cpp
)

target_link_libraries(common-lib PUBLIC sfml-system sfml-graphics sfml-network)
