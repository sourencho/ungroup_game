add_executable (ug-server
    main.cpp
    systems/NetworkingServer.cpp
    systems/ServerGameController.cpp
)

target_link_libraries(ug-server PRIVATE common)
