# Download and configure Catch2 for the tests
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG master
)

FetchContent_GetProperties(catch2)
if (NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
    include(Catch)
endif()

add_executable(ug-test
    tests-main.cpp
    common/objects/tests-circle.cpp
    common/events/tests-EventController.cpp
)

target_link_libraries(ug-test PRIVATE common Catch2::Catch2)
