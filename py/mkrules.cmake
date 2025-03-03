# CMake fragment for MicroPython rules

set(MICROPY_GENHDR_DIR "${CMAKE_BINARY_DIR}/genhdr")
set(MICROPY_MPVERSION "${MICROPY_GENHDR_DIR}/mpversion.h")
set(MICROPY_QSTRDEFS_PY "${MICROPY_PY_DIR}/qstrdefs.h")
set(MICROPY_QSTRDEFS_LAST "${MICROPY_GENHDR_DIR}/qstr.i.last")
set(MICROPY_QSTRDEFS_SPLIT "${MICROPY_GENHDR_DIR}/qstr.split")
set(MICROPY_QSTRDEFS_COLLECTED "${MICROPY_GENHDR_DIR}/qstrdefs.collected.h")
set(MICROPY_QSTRDEFS_PREPROCESSED "${MICROPY_GENHDR_DIR}/qstrdefs.preprocessed.h")
set(MICROPY_QSTRDEFS_GENERATED "${MICROPY_GENHDR_DIR}/qstrdefs.generated.h")
set(MICROPY_MODULEDEFS_SPLIT "${MICROPY_GENHDR_DIR}/moduledefs.split")
set(MICROPY_MODULEDEFS_COLLECTED "${MICROPY_GENHDR_DIR}/moduledefs.collected")
set(MICROPY_MODULEDEFS "${MICROPY_GENHDR_DIR}/moduledefs.h")
set(MICROPY_ROOT_POINTERS_SPLIT "${MICROPY_GENHDR_DIR}/root_pointers.split")
set(MICROPY_ROOT_POINTERS_COLLECTED "${MICROPY_GENHDR_DIR}/root_pointers.collected")
set(MICROPY_ROOT_POINTERS "${MICROPY_GENHDR_DIR}/root_pointers.h")

if(NOT MICROPY_PREVIEW_VERSION_2)
    set(MICROPY_PREVIEW_VERSION_2 0)
endif()

# Set the board name.
if(MICROPY_BOARD)
    if(MICROPY_BOARD_VARIANT)
        set(MICROPY_BOARD_BUILD_NAME ${MICROPY_BOARD}-${MICROPY_BOARD_VARIANT})
    else()
        set(MICROPY_BOARD_BUILD_NAME ${MICROPY_BOARD})
    endif()

    target_compile_definitions(${MICROPY_TARGET} PRIVATE
        MICROPY_BOARD_BUILD_NAME="${MICROPY_BOARD_BUILD_NAME}"
    )
endif()

# Need to do this before extracting MICROPY_CPP_DEF below. Rest of frozen
# manifest handling is at the end of this file.
if(MICROPY_FROZEN_MANIFEST)
    target_compile_definitions(${MICROPY_TARGET} PUBLIC
        MICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
        MICROPY_MODULE_FROZEN_MPY=\(1\)
    )
endif()

if(MICROPY_PREVIEW_VERSION_2)
    target_compile_definitions(${MICROPY_TARGET} PUBLIC
        MICROPY_PREVIEW_VERSION_2=\(1\)
    )
endif()

# Provide defaults for preprocessor flags if not already defined
if(NOT MICROPY_CPP_FLAGS)
    get_target_property(MICROPY_CPP_INC ${MICROPY_TARGET} INCLUDE_DIRECTORIES)
    get_target_property(MICROPY_CPP_DEF ${MICROPY_TARGET} COMPILE_DEFINITIONS)
endif()

# Compute MICROPY_CPP_FLAGS for preprocessor
list(APPEND MICROPY_CPP_INC ${MICROPY_CPP_INC_EXTRA})
list(APPEND MICROPY_CPP_DEF ${MICROPY_CPP_DEF_EXTRA})
set(_prefix "-I")
foreach(_arg ${MICROPY_CPP_INC})
    list(APPEND MICROPY_CPP_FLAGS ${_prefix}${_arg})
endforeach()
set(_prefix "-D")
foreach(_arg ${MICROPY_CPP_DEF})
    list(APPEND MICROPY_CPP_FLAGS ${_prefix}${_arg})
endforeach()
list(APPEND MICROPY_CPP_FLAGS ${MICROPY_CPP_FLAGS_EXTRA})

# Include anything passed in via CFLAGS_EXTRA
# in both MICROPY_CPP_FLAGS and CMAKE_C_FLAGS
if(DEFINED ENV{CFLAGS_EXTRA})
  set(CFLAGS_EXTRA $ENV{CFLAGS_EXTRA})
  string(APPEND CMAKE_C_FLAGS " ${CFLAGS_EXTRA}")  # ... not a list
  separate_arguments(CFLAGS_EXTRA)
  list(APPEND MICROPY_CPP_FLAGS ${CFLAGS_EXTRA})  # ... a list
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

target_sources(${MICROPY_TARGET} PRIVATE
    ${MICROPY_MPVERSION}
    ${MICROPY_QSTRDEFS_GENERATED}
    ${MICROPY_MODULEDEFS}
    ${MICROPY_ROOT_POINTERS}
)

# Command to force the build of another command

# Generate mpversion.h

add_custom_target(
    BUILD_VERSION_HEADER ALL
    BYPRODUCTS ${MICROPY_MPVERSION}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MICROPY_GENHDR_DIR}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_DIR}/py/makeversionhdr.py ${MICROPY_MPVERSION}
)

# Generate qstrs

# If any of the dependencies in this rule change then the C-preprocessor step must be run.
# It only needs to be passed the list of MICROPY_SOURCE_QSTR files that have changed since
# it was last run, but it looks like it's not possible to specify that with cmake.
add_custom_command(
    OUTPUT ${MICROPY_QSTRDEFS_LAST}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makeqstrdefs.py pp ${CMAKE_C_COMPILER} -E output ${MICROPY_GENHDR_DIR}/qstr.i.last cflags ${MICROPY_CPP_FLAGS} -DNO_QSTR cxxflags ${MICROPY_CPP_FLAGS} -DNO_QSTR sources ${MICROPY_SOURCE_QSTR}
    DEPENDS ${MICROPY_MPVERSION}
        ${MICROPY_SOURCE_QSTR}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    OUTPUT ${MICROPY_QSTRDEFS_SPLIT}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makeqstrdefs.py split qstr ${MICROPY_GENHDR_DIR}/qstr.i.last ${MICROPY_GENHDR_DIR}/qstr _
    COMMAND touch ${MICROPY_QSTRDEFS_SPLIT}
    DEPENDS ${MICROPY_QSTRDEFS_LAST}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    OUTPUT ${MICROPY_QSTRDEFS_COLLECTED}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makeqstrdefs.py cat qstr _ ${MICROPY_GENHDR_DIR}/qstr ${MICROPY_QSTRDEFS_COLLECTED}
    BYPRODUCTS "${MICROPY_QSTRDEFS_COLLECTED}.hash"
    DEPENDS ${MICROPY_QSTRDEFS_SPLIT}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    OUTPUT ${MICROPY_QSTRDEFS_PREPROCESSED}
    COMMAND cat ${MICROPY_QSTRDEFS_PY} ${MICROPY_QSTRDEFS_PORT} ${MICROPY_QSTRDEFS_COLLECTED} | sed "s/^Q(.*)/\"&\"/" | ${CMAKE_C_COMPILER} -E ${MICROPY_CPP_FLAGS} - | sed "s/^\\\"\\(Q(.*)\\)\\\"/\\1/" > ${MICROPY_QSTRDEFS_PREPROCESSED}
    DEPENDS ${MICROPY_QSTRDEFS_PY}
        ${MICROPY_QSTRDEFS_PORT}
        ${MICROPY_QSTRDEFS_COLLECTED}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    OUTPUT ${MICROPY_QSTRDEFS_GENERATED}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makeqstrdata.py ${MICROPY_QSTRDEFS_PREPROCESSED} > ${MICROPY_QSTRDEFS_GENERATED}
    DEPENDS ${MICROPY_QSTRDEFS_PREPROCESSED}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

# Generate moduledefs.h

add_custom_command(
    OUTPUT ${MICROPY_MODULEDEFS_SPLIT}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makeqstrdefs.py split module ${MICROPY_GENHDR_DIR}/qstr.i.last ${MICROPY_GENHDR_DIR}/module _
    COMMAND touch ${MICROPY_MODULEDEFS_SPLIT}
    DEPENDS ${MICROPY_QSTRDEFS_LAST}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    OUTPUT ${MICROPY_MODULEDEFS_COLLECTED}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makeqstrdefs.py cat module _ ${MICROPY_GENHDR_DIR}/module ${MICROPY_MODULEDEFS_COLLECTED}
    BYPRODUCTS "${MICROPY_MODULEDEFS_COLLECTED}.hash"
    DEPENDS ${MICROPY_MODULEDEFS_SPLIT}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    OUTPUT ${MICROPY_MODULEDEFS}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makemoduledefs.py ${MICROPY_MODULEDEFS_COLLECTED} > ${MICROPY_MODULEDEFS}
    DEPENDS ${MICROPY_MODULEDEFS_COLLECTED}
)

# Generate root_pointers.h

add_custom_command(
    OUTPUT ${MICROPY_ROOT_POINTERS_SPLIT}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makeqstrdefs.py split root_pointer ${MICROPY_GENHDR_DIR}/qstr.i.last ${MICROPY_GENHDR_DIR}/root_pointer _
    COMMAND touch ${MICROPY_ROOT_POINTERS_SPLIT}
    DEPENDS ${MICROPY_QSTRDEFS_LAST}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    OUTPUT ${MICROPY_ROOT_POINTERS_COLLECTED}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makeqstrdefs.py cat root_pointer _ ${MICROPY_GENHDR_DIR}/root_pointer ${MICROPY_ROOT_POINTERS_COLLECTED}
    BYPRODUCTS "${MICROPY_ROOT_POINTERS_COLLECTED}.hash"
    DEPENDS ${MICROPY_ROOT_POINTERS_SPLIT}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    OUTPUT ${MICROPY_ROOT_POINTERS}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/make_root_pointers.py ${MICROPY_ROOT_POINTERS_COLLECTED} > ${MICROPY_ROOT_POINTERS}
    DEPENDS ${MICROPY_ROOT_POINTERS_COLLECTED} ${MICROPY_PY_DIR}/make_root_pointers.py
)

# Build frozen code if enabled

if(MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_CONTENT "${CMAKE_BINARY_DIR}/frozen_content.c")

    target_sources(${MICROPY_TARGET} PRIVATE
        ${MICROPY_FROZEN_CONTENT}
    )

    # Note: target_compile_definitions already added earlier.

    if(NOT MICROPY_LIB_DIR)
        string(CONCAT GIT_SUBMODULES "${GIT_SUBMODULES} " lib/micropython-lib)
        set(MICROPY_LIB_DIR ${MICROPY_DIR}/lib/micropython-lib)
    endif()

    if(ECHO_SUBMODULES)
        # No-op, we're just doing submodule/variant discovery.
        # Note: All the following rules are safe to run in discovery mode even
        # though the submodule might not be available as they do not directly depend
        # on anything from the submodule.
    elseif(NOT EXISTS ${MICROPY_LIB_DIR}/README.md)
        message(FATAL_ERROR " micropython-lib not initialized.\n Run 'make BOARD=${MICROPY_BOARD} submodules'")
    endif()

    # If MICROPY_MPYCROSS is not explicitly defined in the environment (which
    # is what makemanifest.py will use) then create an mpy-cross dependency
    # to automatically build mpy-cross if needed.
    set(MICROPY_MPYCROSS $ENV{MICROPY_MPYCROSS})
    if(NOT MICROPY_MPYCROSS)
        set(MICROPY_MPYCROSS_DEPENDENCY ${MICROPY_DIR}/mpy-cross/build/mpy-cross)
        if(NOT MICROPY_MAKE_EXECUTABLE)
            set(MICROPY_MAKE_EXECUTABLE make)
        endif()
        add_custom_command(
            OUTPUT ${MICROPY_MPYCROSS_DEPENDENCY}
            COMMAND ${MICROPY_MAKE_EXECUTABLE} -C ${MICROPY_DIR}/mpy-cross USER_C_MODULES=
        )
    endif()

    if(NOT MICROPY_CROSS_FLAGS)
        set(MICROPY_CROSS_FLAGS "")
    else()
        set(MICROPY_CROSS_FLAGS "-f${MICROPY_CROSS_FLAGS}")
    endif()

    # Set default path variables to be passed to makemanifest.py. These will
    # be available in path substitutions. Additional variables can be set
    # per-board in mpconfigboard.cmake.
    set(MICROPY_MANIFEST_PORT_DIR ${MICROPY_PORT_DIR})
    set(MICROPY_MANIFEST_BOARD_DIR ${MICROPY_BOARD_DIR})
    set(MICROPY_MANIFEST_MPY_DIR ${MICROPY_DIR})
    set(MICROPY_MANIFEST_MPY_LIB_DIR ${MICROPY_LIB_DIR})

    # Find all MICROPY_MANIFEST_* variables and turn them into command line arguments.
    get_cmake_property(_manifest_vars VARIABLES)
    list(FILTER _manifest_vars INCLUDE REGEX "MICROPY_MANIFEST_.*")
    foreach(_manifest_var IN LISTS _manifest_vars)
        list(APPEND _manifest_var_args "-v")
        string(REGEX REPLACE "MICROPY_MANIFEST_(.*)" "\\1" _manifest_var_name ${_manifest_var})
        list(APPEND _manifest_var_args "${_manifest_var_name}=${${_manifest_var}}")
    endforeach()

    add_custom_target(
        BUILD_FROZEN_CONTENT ALL
        BYPRODUCTS ${MICROPY_FROZEN_CONTENT}
        COMMAND ${Python3_EXECUTABLE} ${MICROPY_DIR}/tools/makemanifest.py -o ${MICROPY_FROZEN_CONTENT} ${_manifest_var_args} -b "${CMAKE_BINARY_DIR}" ${MICROPY_CROSS_FLAGS} --mpy-tool-flags=${MICROPY_MPY_TOOL_FLAGS} ${MICROPY_FROZEN_MANIFEST}
        DEPENDS
            ${MICROPY_QSTRDEFS_GENERATED}
            ${MICROPY_ROOT_POINTERS}
            ${MICROPY_MPYCROSS_DEPENDENCY}
        VERBATIM
    )
endif()

# Update submodules
if(ECHO_SUBMODULES)
    # If cmake is run with GIT_SUBMODULES defined on command line, process the port / board
    # settings then print the final GIT_SUBMODULES variable and exit.
    # Note: the GIT_SUBMODULES is done via echo rather than message, as message splits
    # the output onto multiple lines
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "GIT_SUBMODULES=${GIT_SUBMODULES}")
    message(FATAL_ERROR "Done")
endif()
