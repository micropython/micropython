# Create an INTERFACE library for our C modules
add_library(usermod_advanced_examples INTERFACE)

# Add our source files to the library
target_sources(usermod_advanced_examples INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/buffer_example.c
    ${CMAKE_CURRENT_LIST_DIR}/callback_example.c
)

# Add the current directory to the include path
target_include_directories(usermod_advanced_examples INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our library to the usermod target
target_link_libraries(usermod INTERFACE usermod_advanced_examples)
