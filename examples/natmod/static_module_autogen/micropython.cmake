# CMake configuration for dual-mode module as user C module

# Add source files
add_library(usermod_dualmode_simple INTERFACE)

target_sources(usermod_dualmode_simple INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/dualmode_simple.c
)

# Add include directory for mp_dualmode.h
target_include_directories(usermod_dualmode_simple INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link to MicroPython core
target_link_libraries(usermod_dualmode_simple INTERFACE usermod)

# For the more complex example
add_library(usermod_dualmode_example INTERFACE)

target_sources(usermod_dualmode_example INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/dualmode_example.c
)

target_include_directories(usermod_dualmode_example INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(usermod_dualmode_example INTERFACE usermod)