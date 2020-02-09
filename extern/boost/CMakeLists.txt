# Header only library, therefore INTERFACE
add_library(boost INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(boost INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/circular_buffer)
