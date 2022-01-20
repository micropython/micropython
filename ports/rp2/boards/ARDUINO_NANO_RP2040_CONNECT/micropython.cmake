# Create an INTERFACE library for our C module.
add_library(nano_modules INTERFACE)

target_compile_definitions(nano_modules INTERFACE
    MICROPY_PY_AUDIO=1
    USE_LUT
)

# Add the current directory as an include directory.
target_include_directories(nano_modules INTERFACE
    ${MICROPY_DIR}/lib/openpdm
)

# Add our source files to the lib
target_sources(nano_modules INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/audio/modaudio.c
    ${MICROPY_DIR}/lib/openpdm/pdm_filter.c
)

# PDM PIO program
pico_generate_pio_header(nano_modules
    ${CMAKE_CURRENT_LIST_DIR}/audio/pdm.pio
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE nano_modules)
