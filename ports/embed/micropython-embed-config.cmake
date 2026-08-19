# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2022-2023 Damien P. George
#
# CMake package configuration for the MicroPython embed port.
#
# This builds MicroPython (the core "py" component plus the embed port glue)
# as a library that can be linked into a wider CMake project. It is an
# alternative to the `embed.mk` flow, which instead copies a self-contained
# tree of source files out of this repository.
#
# A consuming project uses it like this:
#
#     # Directory containing the project's mpconfigport.h.
#     set(MICROPY_EMBED_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR})
#
#     find_package(micropython-embed CONFIG REQUIRED PATHS /path/to/ports/embed)
#
#     add_executable(my_app main.c)
#     target_link_libraries(my_app PRIVATE micropython::micropython)
#
# Variables that may be set before find_package() to configure the build:
#
#     MICROPY_EMBED_CONFIG_DIR    (required) directory containing mpconfigport.h
#     MICROPY_EMBED_LIBRARY_TYPE  STATIC (default) or SHARED

# Guard against being included more than once (e.g. by several consumers in the
# same build tree). The second inclusion would fail on the duplicate target.
include_guard(GLOBAL)

# The directory containing this config file is the embed port directory.
set(MICROPY_EMBED_PORT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(MICROPY_PORT_DIR ${MICROPY_EMBED_PORT_DIR}/port)

# Locate the top of the MicroPython repository (two levels up from ports/embed).
# Resolved relative to this file so it is independent of the consumer's location.
get_filename_component(MICROPY_DIR "${MICROPY_EMBED_PORT_DIR}/../.." ABSOLUTE)

# The consuming project must tell us where to find its mpconfigport.h.
if(NOT MICROPY_EMBED_CONFIG_DIR)
    message(FATAL_ERROR
        "MICROPY_EMBED_CONFIG_DIR is not set.\n"
        "Set it to the directory containing your mpconfigport.h before calling "
        "find_package(micropython-embed), for example:\n"
        "  set(MICROPY_EMBED_CONFIG_DIR \${CMAKE_CURRENT_LIST_DIR})")
endif()

# Library target name, and its type. Default to a static library, which is the
# natural result for embedding MicroPython into another program; set
# MICROPY_EMBED_LIBRARY_TYPE to SHARED before find_package() to override.
set(MICROPY_TARGET micropython)
if(NOT MICROPY_EMBED_LIBRARY_TYPE)
    set(MICROPY_EMBED_LIBRARY_TYPE STATIC)
endif()

# Bring in the core py source list (defines MICROPY_SOURCE_PY).
include(${MICROPY_DIR}/py/py.cmake)

# The embed port's own glue sources.
set(MICROPY_SOURCE_EMBED
    ${MICROPY_PORT_DIR}/embed_util.c
    ${MICROPY_PORT_DIR}/mphalport.c
    ${MICROPY_DIR}/shared/runtime/gchelper_generic.c
    ${MICROPY_DIR}/shared/timeutils/timeutils.c
)

add_library(${MICROPY_TARGET} ${MICROPY_EMBED_LIBRARY_TYPE}
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EMBED}
)

# Namespaced alias so consumers can link against the conventional form.
add_library(micropython::micropython ALIAS ${MICROPY_TARGET})

target_compile_options(${MICROPY_TARGET} PRIVATE -std=gnu99)

# mkrules.cmake reads the target's COMPILE_DEFINITIONS property to build the
# qstr preprocessor flags. If the target has no definitions at all the property
# reads back as "<VAR>-NOTFOUND" and corrupts the flags, so ensure at least one
# definition is always present. MICROPY_EMBED doubles as a marker macro that
# embedding code can test for.
target_compile_definitions(${MICROPY_TARGET} PRIVATE MICROPY_EMBED=1)

target_include_directories(${MICROPY_TARGET} PUBLIC
    ${MICROPY_DIR}                  # core headers, e.g. extmod/*.h
    ${MICROPY_EMBED_PORT_DIR}       # for port/*.h (mphalport.h, micropython_embed.h)
    ${MICROPY_EMBED_CONFIG_DIR}     # for the project's mpconfigport.h
    ${CMAKE_BINARY_DIR}             # for generated genhdr/*.h
)

# Sources whose qstrs and module definitions must be collected.
set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EMBED}
)

# Bring in the qstr/genhdr generation rules and wire up the build.
include(${MICROPY_DIR}/py/mkrules.cmake)
