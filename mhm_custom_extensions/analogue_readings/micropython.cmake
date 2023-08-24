# Create an INTERFACE library for our module.
add_library(analogue_readings INTERFACE)

# Add our source files to the lib
target_sources(analogue_readings INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/analogue_readings.cpp
    ${CMAKE_CURRENT_LIST_DIR}/module_config.c
)

# Add the current directory as an include directory.
target_include_directories(analogue_readings INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE analogue_readings)
