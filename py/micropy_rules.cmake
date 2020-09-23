# CMake fragment for MicroPython rules

target_sources(${MICROPYTHON_TARGET} PRIVATE
    ${MPY_MPVERSION}
    ${MPY_QSTR_DEFS_GENERATED}
    ${MPY_FROZEN_CONTENT}
)

# Generate genhdr directory

add_custom_command(
    OUTPUT ${MPY_GENHDR_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MPY_GENHDR_DIR}
)

# Generate mpversion.h

add_custom_command(
    OUTPUT ${MPY_MPVERSION}
    COMMAND python3 ${MPY_DIR}/py/makeversionhdr.py ${MPY_MPVERSION}
    DEPENDS ${MPY_GENHDR_DIR}
)

# Generate moduledefs.h
# This is currently hard-coded to support modarray.c only, because makemoduledefs.py doesn't support absolute paths

add_custom_command(
    OUTPUT ${MPY_MODULEDEFS}
    COMMAND python3 ${MPY_PY_DIR}/makemoduledefs.py --vpath="." "${MPY_PY_DIR}/modarray.c" > ${MPY_MODULEDEFS}
    DEPENDS ${MPY_MPVERSION}
        ${SOURCE_QSTR}
)

# Generate qstrs

# If any of the dependencies in this rule change then the C-preprocessor step (the first
# command) must be run.  It only needs to be passed the list of SOURCE_QSTR files that
# have changed since it was last run, but it looks like it's not possible to specify that
# with cmake.  The output MPY_QSTR_DEFS_COLLECTED will only be updated if qstrs have changed,
# in which case all source files that use qstrs will be recompiled.
add_custom_command(
    OUTPUT ${MPY_QSTR_DEFS_COLLECTED}
    COMMAND ${CMAKE_C_COMPILER} -E \$\(C_INCLUDES\) \$\(C_FLAGS\) -DNO_QSTR ${SOURCE_QSTR} > ${MPY_GENHDR_DIR}/qstr.i.last
    COMMAND python3 ${MPY_DIR}/py/makeqstrdefs.py split qstr ${MPY_GENHDR_DIR}/qstr.i.last ${MPY_GENHDR_DIR}/qstr _
    COMMAND python3 ${MPY_DIR}/py/makeqstrdefs.py cat qstr _ ${MPY_GENHDR_DIR}/qstr ${MPY_QSTR_DEFS_COLLECTED}
    DEPENDS ${MPY_GENHDR_DIR}
        ${SOURCE_QSTR}
        ${MPY_MODULEDEFS}
)

add_custom_command(
    OUTPUT ${MPY_QSTR_DEFS_PREPROCESSED}
    COMMAND cat ${MPY_PY_QSTRDEFS} ${MPY_QSTR_DEFS_COLLECTED} | sed "'s/^Q(.*)/\"&\"/'" | ${CMAKE_C_COMPILER} -E \$\(C_INCLUDES\) \$\(C_FLAGS\) - | sed "'s/^\\\"\\(Q(.*)\\)\\\"/\\1/'" > ${MPY_QSTR_DEFS_PREPROCESSED}
    DEPENDS ${MPY_QSTR_DEFS_COLLECTED}
)

add_custom_command(
    OUTPUT ${MPY_QSTR_DEFS_GENERATED}
    COMMAND python3 ${MPY_PY_DIR}/makeqstrdata.py ${MPY_QSTR_DEFS_PREPROCESSED} > ${MPY_QSTR_DEFS_GENERATED}
    DEPENDS ${MPY_QSTR_DEFS_PREPROCESSED}
    VERBATIM
)

# Build frozen code

target_compile_options(${MICROPYTHON_TARGET} PUBLIC
    -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
    -DMICROPY_MODULE_FROZEN_MPY=\(1\)
)

add_custom_command(
    OUTPUT ${MPY_FROZEN_CONTENT}
    COMMAND python3 ${MPY_DIR}/tools/makemanifest.py -o ${MPY_FROZEN_CONTENT} -v "MPY_DIR=${MPY_DIR}" -v "PORT_DIR=${MPY_PORT_DIR}" -b "${CMAKE_BINARY_DIR}" -f${MPY_CROSS_FLAGS} ${FROZEN_MANIFEST}
    DEPENDS ${MPY_QSTR_DEFS_GENERATED}
    VERBATIM
)
