# Create a target for all user modules to link against.
add_library(usermod INTERFACE)

function(usermod_gather_sources SOURCES_VARNAME INCLUDE_DIRECTORIES_VARNAME INCLUDED_VARNAME LIB)
    if (NOT ${LIB} IN_LIST ${INCLUDED_VARNAME})
        list(APPEND ${INCLUDED_VARNAME} ${LIB})

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

# Include CMake files for user modules.
if (USER_C_MODULES)
    foreach(USER_C_MODULE_PATH ${USER_C_MODULES})
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
