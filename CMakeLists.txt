cmake_minimum_required(VERSION 3.14)

# set the project name, version and language:w
project(Ungroup VERSION 1.0 LANGUAGES CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

# find sfml
find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)
