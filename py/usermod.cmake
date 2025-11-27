# Create a target for all user modules to link against.
add_library(usermod INTERFACE)

function(usermod_gather_sources SOURCES_VARNAME INCLUDE_DIRECTORIES_VARNAME INCLUDED_VARNAME LIB)
    if (NOT ${LIB} IN_LIST ${INCLUDED_VARNAME})
        list(APPEND ${INCLUDED_VARNAME} ${LIB})

        if (NOT TARGET ${LIB})
            return()
        endif()

        # Gather library sources
        get_target_property(lib_sources ${LIB} INTERFACE_SOURCES)
        if (lib_sources)
            list(APPEND ${SOURCES_VARNAME} ${lib_sources})
        endif()

        # Gather library includes
        get_target_property(lib_include_directories ${LIB} INTERFACE_INCLUDE_DIRECTORIES)
        if (lib_include_directories)
            list(APPEND ${INCLUDE_DIRECTORIES_VARNAME} ${lib_include_directories})
        endif()

        # Recurse linked libraries
        get_target_property(trans_depend ${LIB} INTERFACE_LINK_LIBRARIES)
        if (trans_depend)
            foreach(SUB_LIB ${trans_depend})
                usermod_gather_sources(
                    ${SOURCES_VARNAME}
                    ${INCLUDE_DIRECTORIES_VARNAME}
                    ${INCLUDED_VARNAME}
                    ${SUB_LIB})
            endforeach()
        endif()

        set(${SOURCES_VARNAME} ${${SOURCES_VARNAME}} PARENT_SCOPE)
        set(${INCLUDE_DIRECTORIES_VARNAME} ${${INCLUDE_DIRECTORIES_VARNAME}} PARENT_SCOPE)
        set(${INCLUDED_VARNAME} ${${INCLUDED_VARNAME}} PARENT_SCOPE)
    endif()
endfunction()

# Extract C module paths from manifest if MICROPY_FROZEN_MANIFEST is set.
if (MICROPY_FROZEN_MANIFEST)
    # MICROPY_LIB_DIR is set in py.cmake (included before this file).
    # Set manifest variables for makemanifest.py.
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

    # Find all MICROPY_MANIFEST_* variables and turn them into command line arguments.
    get_cmake_property(_manifest_vars VARIABLES)
    list(FILTER _manifest_vars INCLUDE REGEX "MICROPY_MANIFEST_.*")
    set(_manifest_var_args)
    foreach(_manifest_var IN LISTS _manifest_vars)
        list(APPEND _manifest_var_args "-v")
        string(REGEX REPLACE "MICROPY_MANIFEST_(.*)" "\\1" _manifest_var_name ${_manifest_var})
        list(APPEND _manifest_var_args "${_manifest_var_name}=${${_manifest_var}}")
    endforeach()

    # Extract C module paths from manifest if it exists.
    if (EXISTS ${MICROPY_FROZEN_MANIFEST})
        # Find Python if not already found.
        if (NOT Python3_EXECUTABLE)
            find_package(Python3 REQUIRED COMPONENTS Interpreter)
        endif()

        execute_process(
            COMMAND "${Python3_EXECUTABLE}" "${MICROPY_DIR}/tools/makemanifest.py"
                --list-c-modules ${_manifest_var_args} "${MICROPY_FROZEN_MANIFEST}"
            OUTPUT_VARIABLE MANIFEST_C_MODULES
            ERROR_VARIABLE MANIFEST_ERROR
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE MANIFEST_RESULT
        )

        # Check if the command succeeded.
        if (NOT MANIFEST_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to extract C modules from manifest: ${MICROPY_FROZEN_MANIFEST}\nError: ${MANIFEST_ERROR}")
        endif()

        # Append manifest C modules to USER_C_MODULES list.
        if (MANIFEST_C_MODULES)
            # Convert space-separated string to CMake list.
            string(REPLACE " " ";" MANIFEST_C_MODULES_LIST "${MANIFEST_C_MODULES}")
            list(APPEND USER_C_MODULES ${MANIFEST_C_MODULES_LIST})
        endif()
    endif()
endif()

# Include CMake files for user modules.
if (USER_C_MODULES)
    foreach(USER_C_MODULE_PATH ${USER_C_MODULES})
        # If a directory is given, append the micropython.cmake to it.
        if (IS_DIRECTORY ${USER_C_MODULE_PATH})
            set(USER_C_MODULE_PATH "${USER_C_MODULE_PATH}/micropython.cmake")
        endif()
        # Confirm the provided path exists, show abspath if not to make it clearer to fix.
        if (NOT EXISTS ${USER_C_MODULE_PATH})
            get_filename_component(USER_C_MODULES_ABS "${USER_C_MODULE_PATH}" ABSOLUTE)
            message(FATAL_ERROR "USER_C_MODULES doesn't exist: ${USER_C_MODULES_ABS}")
        endif()

        message("Including User C Module(s) from ${USER_C_MODULE_PATH}")
        include(${USER_C_MODULE_PATH})
    endforeach()
endif()

# Recursively gather sources for QSTR scanning - doesn't support generator expressions.
usermod_gather_sources(MICROPY_SOURCE_USERMOD MICROPY_INC_USERMOD found_modules usermod)

# Report found modules.
list(REMOVE_ITEM found_modules "usermod")
list(JOIN found_modules ", " found_modules)
message("Found User C Module(s): ${found_modules}")
