cmake_minimum_required(VERSION 3.11..14)

# set the project name, version and language:w
project(Ungroup VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(UNGROUP_STATIC)
    # todo(souren|#192|2020-04-24): Unable to build with sfml statically linked on mac/linux 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++ -static-libgcc -static-libstdc++ -static")
    set(CMAKE_FIND_FRAMEWORK LAST)
    set(SFML_STATIC_LIBRARIES TRUE)
endif()

# find threads
find_package(Threads REQUIRED)

# find sfml
find_package(SFML 2.5.1 COMPONENTS system window graphics network audio REQUIRED)

# find boost
find_package(Boost 1.55 COMPONENTS REQUIRED)

add_subdirectory(src)
add_subdirectory(extern/catch)
add_subdirectory(extern/cxxopts)
add_subdirectory(tests)
