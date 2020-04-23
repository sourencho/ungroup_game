add_executable (ug-server
    main.cpp
)

add_library(server-lib
    systems/NetworkingServer.cpp
    systems/ServerGameController.cpp
)

target_link_libraries(server-lib common-lib sfml-window sfml-graphics sfml-network cxxopts)
target_link_libraries(ug-server server-lib)