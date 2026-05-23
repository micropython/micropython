# Extract c_module() entries from MICROPY_FROZEN_MANIFEST and append them to
# USER_C_MODULES. Included from py/usermod.cmake; the port must have resolved
# MICROPY_FROZEN_MANIFEST (and any board-config default / cmake-command-line
# override) before usermod.cmake runs.
#
# Also sets default MICROPY_MANIFEST_* path variables that are forwarded to
# makemanifest.py here for c_module extraction and again from py/mkrules.cmake
# for the frozen content generation step, so both passes substitute the same
# values into manifest path expressions.

if(MICROPY_FROZEN_MANIFEST)
    # Set default path variables to be passed to makemanifest.py. These are
    # available in path substitutions inside the manifest. Additional variables
    # can be set per-board in mpconfigboard.cmake or on the cmake command line.
    # MICROPY_LIB_DIR is set in py.cmake (included before usermod.cmake).
    if(NOT DEFINED MICROPY_MANIFEST_PORT_DIR)
        set(MICROPY_MANIFEST_PORT_DIR ${MICROPY_PORT_DIR})
    endif()
    if(NOT DEFINED MICROPY_MANIFEST_BOARD_DIR)
        set(MICROPY_MANIFEST_BOARD_DIR ${MICROPY_BOARD_DIR})
    endif()
    if(NOT DEFINED MICROPY_MANIFEST_MPY_DIR)
        set(MICROPY_MANIFEST_MPY_DIR ${MICROPY_DIR})
    endif()
    if(NOT DEFINED MICROPY_MANIFEST_MPY_LIB_DIR)
        set(MICROPY_MANIFEST_MPY_LIB_DIR ${MICROPY_LIB_DIR})
    endif()

    # Find all MICROPY_MANIFEST_* variables and turn them into command line
    # arguments. Stored in MICROPY_MAKEMANIFEST_ARGS (outside the
    # MICROPY_MANIFEST_ namespace) so it doesn't self-match the regex above
    # on subsequent cmake configures.
    get_cmake_property(_manifest_vars VARIABLES)
    list(FILTER _manifest_vars INCLUDE REGEX "MICROPY_MANIFEST_.*")
    set(MICROPY_MAKEMANIFEST_ARGS)
    foreach(_manifest_var IN LISTS _manifest_vars)
        list(APPEND MICROPY_MAKEMANIFEST_ARGS "-v")
        string(REGEX REPLACE "MICROPY_MANIFEST_(.*)" "\\1" _manifest_var_name ${_manifest_var})
        list(APPEND MICROPY_MAKEMANIFEST_ARGS "${_manifest_var_name}=${${_manifest_var}}")
    endforeach()

    # Skip extraction during UPDATE_SUBMODULES (require() would fail) and when
    # micropython-lib is not initialised. Matches py/manifest.mk behaviour:
    # warn and continue with an empty c_module list rather than fatal.
    if(EXISTS ${MICROPY_FROZEN_MANIFEST} AND NOT UPDATE_SUBMODULES)
        if(EXISTS ${MICROPY_LIB_DIR}/README.md)
            if(NOT Python3_EXECUTABLE)
                find_package(Python3 REQUIRED COMPONENTS Interpreter)
            endif()

            execute_process(
                COMMAND "${Python3_EXECUTABLE}" "${MICROPY_DIR}/tools/makemanifest.py"
                    --list-c-modules ${MICROPY_MAKEMANIFEST_ARGS} "${MICROPY_FROZEN_MANIFEST}"
                OUTPUT_VARIABLE MANIFEST_C_MODULES
                ERROR_VARIABLE MANIFEST_ERROR
                OUTPUT_STRIP_TRAILING_WHITESPACE
                RESULT_VARIABLE MANIFEST_RESULT
            )

            if(NOT MANIFEST_RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to extract C modules from manifest: ${MICROPY_FROZEN_MANIFEST}\nError: ${MANIFEST_ERROR}")
            endif()

            if(MANIFEST_C_MODULES)
                string(REPLACE "\n" ";" MANIFEST_C_MODULES_LIST "${MANIFEST_C_MODULES}")
                list(APPEND USER_C_MODULES ${MANIFEST_C_MODULES_LIST})
                list(REMOVE_DUPLICATES USER_C_MODULES)
            endif()
        else()
            message(WARNING "c_module() extraction skipped: micropython-lib not initialised, run 'make BOARD=${MICROPY_BOARD} submodules'")
        endif()
    endif()
endif()
