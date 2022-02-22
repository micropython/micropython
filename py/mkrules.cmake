# CMake fragment for MicroPython rules

set(MICROPY_GENHDR_DIR "${CMAKE_BINARY_DIR}/genhdr")
set(MICROPY_MPVERSION "${MICROPY_GENHDR_DIR}/mpversion.h")
set(MICROPY_MODULEDEFS "${MICROPY_GENHDR_DIR}/moduledefs.h")
set(MICROPY_QSTRDEFS_PY "${MICROPY_PY_DIR}/qstrdefs.h")
set(MICROPY_QSTRDEFS_LAST "${MICROPY_GENHDR_DIR}/qstr.i.last")
set(MICROPY_QSTRDEFS_SPLIT "${MICROPY_GENHDR_DIR}/qstr.split")
set(MICROPY_QSTRDEFS_COLLECTED "${MICROPY_GENHDR_DIR}/qstrdefs.collected.h")
set(MICROPY_QSTRDEFS_PREPROCESSED "${MICROPY_GENHDR_DIR}/qstrdefs.preprocessed.h")
set(MICROPY_QSTRDEFS_GENERATED "${MICROPY_GENHDR_DIR}/qstrdefs.generated.h")

# Need to do this before extracting MICROPY_CPP_DEF below. Rest of frozen
# manifest handling is at the end of this file.
if(MICROPY_FROZEN_MANIFEST)
    target_compile_definitions(${MICROPY_TARGET} PUBLIC
        MICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
        MICROPY_MODULE_FROZEN_MPY=\(1\)
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

find_package(Python3 REQUIRED COMPONENTS Interpreter)

target_sources(${MICROPY_TARGET} PRIVATE
    ${MICROPY_MPVERSION}
    ${MICROPY_QSTRDEFS_GENERATED}
)

# Command to force the build of another command

add_custom_command(
    OUTPUT MICROPY_FORCE_BUILD
    COMMENT ""
    COMMAND echo -n
)

# Generate mpversion.h

add_custom_command(
    OUTPUT ${MICROPY_MPVERSION}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MICROPY_GENHDR_DIR}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_DIR}/py/makeversionhdr.py ${MICROPY_MPVERSION}
    DEPENDS MICROPY_FORCE_BUILD
)

# Generate moduledefs.h

add_custom_command(
    OUTPUT ${MICROPY_MODULEDEFS}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makemoduledefs.py --vpath="/" ${MICROPY_SOURCE_QSTR} > ${MICROPY_MODULEDEFS}
    DEPENDS ${MICROPY_MPVERSION}
        ${MICROPY_SOURCE_QSTR}
)

# Generate qstrs

# If any of the dependencies in this rule change then the C-preprocessor step must be run.
# It only needs to be passed the list of MICROPY_SOURCE_QSTR files that have changed since
# it was last run, but it looks like it's not possible to specify that with cmake.
add_custom_command(
    OUTPUT ${MICROPY_QSTRDEFS_LAST}
    COMMAND ${Python3_EXECUTABLE} ${MICROPY_PY_DIR}/makeqstrdefs.py pp ${CMAKE_C_COMPILER} -E output ${MICROPY_GENHDR_DIR}/qstr.i.last cflags ${MICROPY_CPP_FLAGS} -DNO_QSTR cxxflags ${MICROPY_CPP_FLAGS} -DNO_QSTR sources ${MICROPY_SOURCE_QSTR}
    DEPENDS ${MICROPY_MODULEDEFS}
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

# Build frozen code if enabled

if(MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_CONTENT "${CMAKE_BINARY_DIR}/frozen_content.c")

    target_sources(${MICROPY_TARGET} PRIVATE
        ${MICROPY_FROZEN_CONTENT}
    )

    # Note: target_compile_definitions already added earlier.

    if(NOT MICROPY_LIB_DIR)
        set(MICROPY_LIB_DIR ${MICROPY_DIR}/../micropython-lib)
    endif()

    # If MICROPY_MPYCROSS is not explicitly defined in the environment (which
    # is what makemanifest.py will use) then create an mpy-cross dependency
    # to automatically build mpy-cross if needed.
    set(MICROPY_MPYCROSS $ENV{MICROPY_MPYCROSS})
    if(NOT MICROPY_MPYCROSS)
        set(MICROPY_MPYCROSS_DEPENDENCY ${MICROPY_DIR}/mpy-cross/mpy-cross)
        if(NOT MICROPY_MAKE_EXECUTABLE)
            set(MICROPY_MAKE_EXECUTABLE make)
        endif()
        add_custom_command(
            OUTPUT ${MICROPY_MPYCROSS_DEPENDENCY}
            COMMAND ${MICROPY_MAKE_EXECUTABLE} -C ${MICROPY_DIR}/mpy-cross
        )
    endif()

    add_custom_command(
        OUTPUT ${MICROPY_FROZEN_CONTENT}
        COMMAND ${Python3_EXECUTABLE} ${MICROPY_DIR}/tools/makemanifest.py -o ${MICROPY_FROZEN_CONTENT} -v "MPY_DIR=${MICROPY_DIR}" -v "MPY_LIB_DIR=${MICROPY_LIB_DIR}" -v "PORT_DIR=${MICROPY_PORT_DIR}" -v "BOARD_DIR=${MICROPY_BOARD_DIR}" -b "${CMAKE_BINARY_DIR}" -f${MICROPY_CROSS_FLAGS} ${MICROPY_FROZEN_MANIFEST}
        DEPENDS MICROPY_FORCE_BUILD
            ${MICROPY_QSTRDEFS_GENERATED}
            ${MICROPY_MPYCROSS_DEPENDENCY}
        VERBATIM
    )
endif()
