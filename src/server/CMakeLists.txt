add_executable (ug-server
    main.cpp
)

add_library(server-lib
    systems/NetworkingServer.cpp
    systems/ServerGameController.cpp
)

target_link_libraries(ug-server PRIVATE server-lib common-lib cxxopts)
