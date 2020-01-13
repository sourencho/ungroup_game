add_executable (ug-client
    main.cpp
)

add_library(client-lib
    systems/NetworkingClient.cpp
    systems/ClientGameController.cpp
    systems/InputController.cpp
    rendering/Animation.cpp
    rendering/AnimatedSprite.cpp
    rendering/AnimationController.cpp
    rendering/GUIController.cpp
    rendering/BaseUIElement.cpp
    rendering/TestUIElement.cpp
    rendering/DebugUIElement.cpp
    rendering/ResourceUIElement.cpp
    rendering/RenderingController.cpp
    rendering/BackgroundController.cpp
    rendering/GameObjectRenderer.cpp
    rendering/DrawableGroup.cpp
    rendering/DrawableMine.cpp
    rendering/DirectionArrows.cpp
    rendering/DirectionLines.cpp
    rendering/RenderingUtil.cpp
    rendering/DrawableCircle.cpp
    resources/ResourceStore.cpp
)

target_link_libraries(ug-client PRIVATE client-lib common-lib cxxopts ${GPERFTOOLS_PROFILER})
