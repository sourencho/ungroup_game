add_executable (ug-client
    main.cpp
)

add_library(client-lib
    rendering/Animation.cpp
    rendering/AnimatedSprite.cpp
    rendering/AnimationController.cpp
    systems/NetworkingClient.cpp
    systems/ClientGameController.cpp
    systems/InputController.cpp
)

target_link_libraries(ug-client PRIVATE client-lib common-lib)
