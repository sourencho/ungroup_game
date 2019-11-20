add_library(common-lib
    util/network_util.cpp
    util/StateDef.cpp
    objects/CircleGameObject.cpp
    objects/Player.cpp
    objects/Mine.cpp
    objects/CircleRigidBody.cpp
    objects/Group.cpp
    events/EventController.cpp
    physics/PhysicsController.cpp
    physics/CollisionUtil.cpp
    physics/VectorUtil.cpp
    systems/GameObjectStore.cpp
    systems/GroupController.cpp
    systems/PlayerController.cpp
    systems/MineController.cpp
    systems/GameObjectController.cpp
    systems/GameController.cpp
    factories/IdFactory.cpp
    resources/ResourceStore.cpp
)

target_link_libraries(common-lib PUBLIC sfml-system sfml-graphics sfml-network)
