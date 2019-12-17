# Header only library, therefore INTERFACE
add_library(cxxopts INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(cxxopts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
