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

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_cppexample)

# Do not include stack unwinding & exception handling for C++ user modules
# This can dramatically reduce build size on embedded ports like rp2
# target_compile_definitions(usermod INTERFACE PICO_CXX_ENABLE_EXCEPTIONS=0)
# target_compile_options(usermod INTERFACE $<$<COMPILE_LANGUAGE:CXX>:
#    -fno-exceptions
#    -fno-unwind-tables
#    -fno-rtti
#    -fno-use-cxa-atexit
# >)
# target_link_options(usermod INTERFACE -specs=nano.specs)