add_executable (ug-server
    main.cpp
)

add_library(server-lib
    systems/NetworkingServer.cpp
    systems/ServerGameController.cpp
    rendering/TerminalRenderingController.cpp
    util/TermboxTable.cpp
    util/TermboxTextOutputArea.cpp
)

target_link_libraries(server-lib common-lib sfml-window sfml-graphics sfml-network cxxopts termbox_lib_static)
target_link_libraries(ug-server server-lib)
