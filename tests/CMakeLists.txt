# Testing library
FetchContent_Declare(
catch2
GIT_REPOSITORY https://github.com/catchorg/Catch2.git
GIT_TAG        v2.9.1
)

# Adds Catch2::Catch2
FetchContent_MakeAvailable(catch2)

# Tests need to be added as executables first
add_executable(ug-test
    tests-main.cpp
    common/objects/tests-circle.cpp
    common/events/tests-EventController.cpp
)

target_link_libraries(ug-test PRIVATE common Catch2::Catch2)
