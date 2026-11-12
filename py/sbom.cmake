# micropy_sbom(<image target> <map file> [extra tools/sbom.py arguments...])
#
# Adds the "sbom" target. Call where MICROPY_FROZEN_MANIFEST, MICROPY_MAKEMANIFEST_ARGS
# and USER_C_MODULES are visible, so frozen and user C modules are covered.

function(micropy_sbom image map)
    if(NOT MICROPY_SBOM_TOOL)
        set(MICROPY_SBOM_TOOL uv run --script ${MICROPY_DIR}/tools/sbom.py)
    endif()

    set(args ${ARGN})
    if(MICROPY_FROZEN_MANIFEST)
        list(APPEND args --frozen ${MICROPY_FROZEN_MANIFEST} ${MICROPY_MAKEMANIFEST_ARGS})
    endif()
    # Each user C module entry is a micropython.cmake or a directory holding one. CMake builds
    # the objects by absolute path so the root only decides how the module is grouped.
    foreach(usermod IN LISTS USER_C_MODULES)
        get_filename_component(usermod_abs ${usermod} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        if(NOT IS_DIRECTORY ${usermod_abs})
            get_filename_component(usermod_abs ${usermod_abs} DIRECTORY)
        endif()
        list(APPEND args --src-root ${usermod_abs})
    endforeach()

    add_custom_target(sbom
        COMMAND ${MICROPY_SBOM_TOOL} ${map}
            --spdx ${CMAKE_BINARY_DIR}/sbom.spdx.json --cyclonedx ${CMAKE_BINARY_DIR}/sbom.cdx.json
            ${args}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "SBOM ${CMAKE_BINARY_DIR}/sbom.*.json"
        VERBATIM
    )
    add_dependencies(sbom ${image})
endfunction()
