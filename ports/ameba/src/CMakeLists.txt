##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component
set(MICROPY_PORT_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/..)
# The port lives in-tree at <micropython>/ports/ameba, so the MicroPython core
# is two directories up from the port directory.
set(MICROPY_DIR         ${MICROPY_PORT_DIR}/../..)

# Board selection: prefer cmake -D, then environment variable, then default.
if(NOT MICROPY_BOARD)
    if(DEFINED ENV{MICROPY_BOARD})
        set(MICROPY_BOARD $ENV{MICROPY_BOARD})
    else()
        set(MICROPY_BOARD PKE8721DAF)
    endif()
endif()
if(NOT MICROPY_BOARD_DIR)
    if(DEFINED ENV{MICROPY_BOARD_DIR})
        set(MICROPY_BOARD_DIR $ENV{MICROPY_BOARD_DIR})
    else()
        set(MICROPY_BOARD_DIR ${MICROPY_PORT_DIR}/boards/${MICROPY_BOARD})
    endif()
endif()
if(EXISTS ${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
    include(${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
endif()

include(${MICROPY_DIR}/py/py.cmake)
include(${MICROPY_DIR}/extmod/extmod.cmake)

# Save the manifest file set from the cmake command line.
set(MICROPY_USER_FROZEN_MANIFEST ${MICROPY_FROZEN_MANIFEST})

# Set the frozen manifest file. Note if MICROPY_FROZEN_MANIFEST is set from the cmake
# command line, then it will override the default and any manifest set by the board.
if (MICROPY_USER_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_USER_FROZEN_MANIFEST})
elseif (NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py)
endif()

# Enable error text compression by default.
if(NOT MICROPY_ROM_TEXT_COMPRESSION)
    set(MICROPY_ROM_TEXT_COMPRESSION ON)
endif()

set(MICROPY_SOURCE_SHARED
    ${MICROPY_DIR}/shared/readline/readline.c
    ${MICROPY_DIR}/shared/runtime/gchelper_generic.c
    ${MICROPY_DIR}/shared/runtime/pyexec.c
    ${MICROPY_DIR}/shared/runtime/stdout_helpers.c
    ${MICROPY_DIR}/shared/runtime/interrupt_char.c
    ${MICROPY_DIR}/shared/runtime/sys_stdio_mphal.c
    ${MICROPY_DIR}/shared/timeutils/timeutils.c
)

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_DRIVERS}
    ${MICROPY_SOURCE_PORT}
)


set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)
# You may use if-else condition to set or update predefined variable above

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Template section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries})

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public, it's unnecessary to add again below.

# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files
set(private_includes)                #private include directories
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------------------------------------------#
# Component private part, user config begin(DO NOT remove this line)

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
ameba_list_append(private_sources
    ${MICROPY_SOURCE_PY}
    mp_main.c
    mphalport.c
    help.c
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_EXTMOD}
)
ameba_list_append(private_includes
    ${MICROPY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MICROPY_PORT_DIR}/build
    ${MICROPY_INC_CORE}
    ${MICROPY_INC_USERMOD}
    ${MICROPY_INC_TINYUSB}
    ${MICROPY_PORT_DIR}
    ${MICROPY_BOARD_DIR}
    ${CMAKE_BINARY_DIR}
)

ameba_list_append(private_definitions
    ${MICROPY_DEF_COMPONENT}
    ${MICROPY_DEF_CORE}
    ${MICROPY_DEF_BOARD}
    ${MICROPY_DEF_TINYUSB}
)


ameba_list_append(private_compile_options
    -fbuiltin
    -fno-builtin-memset
    -fno-builtin-memcpy
    -fno-builtin-printf
    # The SDK enables -Wundef, -Wunused-parameter, -Wpointer-arith as errors, but upstream
    # MicroPython code legitimately uses all three patterns:
    #   -Wundef:           #if MACRO where undefined == 0 (__cplusplus, MICROPY_NLR_SETJMP, ...)
    #   -Wunused-parameter: intentional unused params in inline stubs (pystack.h, asmthumb.h)
    #   -Wpointer-arith:   void* arithmetic in gc.c
    # These flags suppress SDK-level warnings for the MicroPython component only.
    # Do NOT remove these flags without auditing every upstream MicroPython file.
    -Wno-undef
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-pointer-arith
    -Wno-sign-compare
)

# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Template section, DO NOT change!
ameba_add_internal_library(micropython
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
    p_DEPENDENCIES
        ${c_BUILD_INFO} #REVIEW: build_info.h is REQUIRED by atcmd_sys.c
)
set(MICROPY_TARGET      ${c_CURRENT_TARGET_NAME})

macro(ameba_gather_target_properties targ)
   if(TARGET ${targ})
       get_target_property(_type ${targ} TYPE)

       if(${_type} STREQUAL "INTERFACE_LIBRARY")
           set(_inc_prop_name "INTERFACE_INCLUDE_DIRECTORIES")
           set(_def_prop_name "INTERFACE_COMPILE_DEFINITIONS")
       else()
           set(_inc_prop_name "INCLUDE_DIRECTORIES")
           set(_def_prop_name "COMPILE_DEFINITIONS")
       endif()

       set(_inc_gen "$<$<BOOL:$<TARGET_PROPERTY:${targ},${_inc_prop_name}>>:-I$<JOIN:$<TARGET_PROPERTY:${targ},${_inc_prop_name}>,$<SEMICOLON>-I>>")
       set(_def_gen "$<$<BOOL:$<TARGET_PROPERTY:${targ},${_def_prop_name}>>:-D$<JOIN:$<TARGET_PROPERTY:${targ},${_def_prop_name}>,$<SEMICOLON>-D>>")

       list(APPEND MICROPY_CPP_FLAGS_EXTRA "${_inc_gen}")
       list(APPEND MICROPY_CPP_FLAGS_EXTRA "${_def_gen}")
   endif()
endmacro()
ameba_gather_target_properties(${c_MCU_PROJ_CONFIG})

include(${MICROPY_DIR}/py/mkrules.cmake)

##########################################################################################
