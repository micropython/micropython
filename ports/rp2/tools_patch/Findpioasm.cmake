# Finds (or builds) the pioasm executable
#
# This will define the following imported targets
#
#     pioasm
#

# This is a temporary patched copy of pico-sdk file Findpioasm.cmake to work around
# a host toolchain issue with GCC 15.1:
# https://github.com/raspberrypi/pico-sdk/issues/2448

if (NOT TARGET pioasm)
    # todo we would like to use pckgconfig to look for it first
    # see https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/

    include(ExternalProject)

    set(PIOASM_SOURCE_DIR ${PICO_SDK_PATH}/tools/pioasm)
    set(PIOASM_BINARY_DIR ${CMAKE_BINARY_DIR}/pioasm)
    set(PIOASM_INSTALL_DIR ${CMAKE_BINARY_DIR}/pioasm-install CACHE PATH "Directory where pioasm has been installed" FORCE)

    set(pioasmBuild_TARGET pioasmBuild)
    set(pioasm_TARGET pioasm)

    if (NOT TARGET ${pioasmBuild_TARGET})
        pico_message_debug("PIOASM will need to be built")
#        message("Adding external project ${pioasmBuild_Target} in ${CMAKE_CURRENT_LIST_DIR}}")
        ExternalProject_Add(${pioasmBuild_TARGET}
                PREFIX pioasm
                SOURCE_DIR ${PIOASM_SOURCE_DIR}
                BINARY_DIR ${PIOASM_BINARY_DIR}
                INSTALL_DIR ${PIOASM_INSTALL_DIR}
                CMAKE_ARGS
                    "--no-warn-unused-cli"
                    "-DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM}"
                    "-DPIOASM_FLAT_INSTALL=1"
                    "-DCMAKE_INSTALL_PREFIX=${PIOASM_INSTALL_DIR}"
                    "-DCMAKE_RULE_MESSAGES=OFF" # quieten the build
                    "-DCMAKE_INSTALL_MESSAGE=NEVER" # quieten the install
                    # Toolchain workaround follows
                    "-DCMAKE_CXX_FLAGS=-include cstdint"
                CMAKE_CACHE_ARGS "-DPIOASM_EXTRA_SOURCE_FILES:STRING=${PIOASM_EXTRA_SOURCE_FILES}"
                BUILD_ALWAYS 1 # force dependency checking
                EXCLUDE_FROM_ALL TRUE
                )
    endif()

    if (CMAKE_HOST_WIN32)
        set(pioasm_EXECUTABLE ${PIOASM_INSTALL_DIR}/pioasm/pioasm.exe)
    else()
        set(pioasm_EXECUTABLE ${PIOASM_INSTALL_DIR}/pioasm/pioasm)
    endif()
    add_executable(${pioasm_TARGET} IMPORTED GLOBAL)
    set_property(TARGET ${pioasm_TARGET} PROPERTY IMPORTED_LOCATION
            ${pioasm_EXECUTABLE})

    add_dependencies(${pioasm_TARGET} ${pioasmBuild_TARGET})
endif()
