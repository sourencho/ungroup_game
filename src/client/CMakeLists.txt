add_executable (ug-client
    main.cpp
    resources/ResourceStore.cpp
    rendering/Animation.cpp
    rendering/AnimatedSprite.cpp
    rendering/AnimationController.cpp
    systems/NetworkingClient.cpp
    systems/ClientGameController.cpp
)

add_library(client-lib
    main.cpp
    resources/ResourceStore.cpp
    rendering/Animation.cpp
    rendering/AnimatedSprite.cpp
    rendering/AnimationController.cpp
    systems/NetworkingClient.cpp
    systems/ClientGameController.cpp
)

target_link_libraries(ug-client PRIVATE common-lib)
