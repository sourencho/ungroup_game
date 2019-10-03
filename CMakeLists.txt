cmake_minimum_required(VERSION 3.11..14)

# set the project name, version and language:w
project(Ungroup VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")

# find sfml
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)

add_subdirectory(src)
add_subdirectory(extern/catch)
add_subdirectory(tests)
