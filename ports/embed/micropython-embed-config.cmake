if (NOT DEFINED MICROPYTHON_EMBED_ONCE)
    set (MICROPYTHON_EMBED_ONCE TRUE)
    set(CMAKE_C_COMPILER "clang")

    set(MICROPY_TARGET micropython)
    get_filename_component(MICROPY_DIR "../.." ABSOLUTE)

    set(MICROPY_PORT_DIR ${CMAKE_CURRENT_LIST_DIR}/port)

    include(${MICROPY_DIR}/py/py.cmake)

    set(MICROPY_SOURCE_EMBED
        ${MICROPY_PORT_DIR}/embed_util.c
        ${MICROPY_PORT_DIR}/mphalport.c
    	${MICROPY_DIR}/shared/runtime/gchelper_generic.c
	    ${MICROPY_DIR}/shared/timeutils/timeutils.c
    )

    add_library(${MICROPY_TARGET} SHARED
        ${MICROPY_SOURCE_PY}
        ${MICROPY_SOURCE_EMBED}
    )

    target_compile_options(${MICROPY_TARGET} PRIVATE -std=gnu99)

    # Must set at least *one* compile definition to stop mkrules.py ->
    # get_target_property(MICROPY_CPP_DEF ${MICROPY_TARGET} COMPILE_DEFINITIONS)
    # failing to "MICROPY_CPP_DEF-NOTFOUND" and causing a compile error.
    target_compile_definitions(${MICROPY_TARGET} PRIVATE DUMMY)

    target_include_directories(${MICROPY_TARGET} PUBLIC
        ${MICROPY_PORT_DIR}
        ${MICROPY_DIR}
        ${MICROPY_EMBED_CONFIG_DIR}
        ${CMAKE_CURRENT_LIST_DIR}          # for port/*.h
        ${CMAKE_CURRENT_BINARY_DIR}        # for genhdr
    )

    set(MICROPY_SOURCE_QSTR
        ${MICROPY_SOURCE_PY}
        ${MICROPY_SOURCE_EMBED}
    )

    include(${MICROPY_DIR}/py/mkrules.cmake)
endif()