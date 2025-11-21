# Create an INTERFACE library for our C module.
add_library(usermod_subpackage_example INTERFACE)

# Add our source files to the lib
target_sources(usermod_subpackage_example INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/examplemodule.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_subpackage_example INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_subpackage_example INTERFACE
    MICROPY_MODULE_BUILTIN_SUBPACKAGES=1
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_subpackage_example)
