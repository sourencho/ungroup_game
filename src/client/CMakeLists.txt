add_executable (ug-client
    main.cpp
    rendering/Animation.cpp
    rendering/AnimatedSprite.cpp
    rendering/AnimationController.cpp
    systems/NetworkingClient.cpp
    systems/ClientGameController.cpp
)

add_library(client-lib
    main.cpp
    rendering/Animation.cpp
    rendering/AnimatedSprite.cpp
    rendering/AnimationController.cpp
    systems/NetworkingClient.cpp
    systems/ClientGameController.cpp
)

target_link_libraries(ug-client PRIVATE common-lib)
