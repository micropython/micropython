# Create an INTERFACE library for our CPP module.
add_library(usermod_cppexample INTERFACE)

# Add our source files to the library.
target_sources(usermod_cppexample INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/example.cpp
    ${CMAKE_CURRENT_LIST_DIR}/examplemodule.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_cppexample INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Enable the module automatically by adding the relevant compile definitions.
target_compile_definitions(usermod_cppexample INTERFACE
    MODULE_CPPEXAMPLE_ENABLED=1
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_cppexample)

