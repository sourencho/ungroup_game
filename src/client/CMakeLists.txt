add_executable (ug-client
    main.cpp
    systems/NetworkingClient.cpp
    systems/ClientGameController.cpp
)

target_link_libraries(ug-client PRIVATE common)
