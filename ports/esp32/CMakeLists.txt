# Top-level cmake file for building MicroPython on ESP32.
#
# Note for maintainers: Where possible, functionality should be put into
# esp32_common.cmake not this file. This is because this CMakeLists.txt file
# needs to be duplicated for out-of-tree builds, and can easily get out of date.
cmake_minimum_required(VERSION 3.12)

# Set the board if it's not already set.
if(NOT MICROPY_BOARD)
    set(MICROPY_BOARD ESP32_GENERIC)
endif()

# Set the board directory and check that it exists.
if(NOT MICROPY_BOARD_DIR)
    set(MICROPY_BOARD_DIR ${CMAKE_CURRENT_LIST_DIR}/boards/${MICROPY_BOARD})
endif()
if(NOT EXISTS ${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
    message(FATAL_ERROR "Invalid MICROPY_BOARD specified: ${MICROPY_BOARD}")
endif()

# If a board variant is specified, check that it exists.
if(MICROPY_BOARD_VARIANT)
    if(NOT EXISTS ${MICROPY_BOARD_DIR}/mpconfigvariant_${MICROPY_BOARD_VARIANT}.cmake)
        message(FATAL_ERROR "Invalid MICROPY_BOARD_VARIANT specified: ${MICROPY_BOARD_VARIANT}")
    endif()
endif()

# Define the output sdkconfig so it goes in the build directory.
set(SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig)

# Save the manifest file set from the cmake command line.
set(MICROPY_USER_FROZEN_MANIFEST ${MICROPY_FROZEN_MANIFEST})

# Include board config; this is expected to set (among other options):
# - SDKCONFIG_DEFAULTS
# - IDF_TARGET
include(${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
if(NOT MICROPY_BOARD_VARIANT)
    include(${MICROPY_BOARD_DIR}/mpconfigvariant.cmake OPTIONAL)
else()
    include(${MICROPY_BOARD_DIR}/mpconfigvariant_${MICROPY_BOARD_VARIANT}.cmake)
endif()

# Set the frozen manifest file. Note if MICROPY_FROZEN_MANIFEST is set from the cmake
# command line, then it will override the default and any manifest set by the board.
if (MICROPY_USER_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_USER_FROZEN_MANIFEST})
elseif (NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${CMAKE_CURRENT_LIST_DIR}/boards/manifest.py)
endif()

# Concatenate all sdkconfig files into a combined one for the IDF to use.
file(WRITE ${CMAKE_BINARY_DIR}/sdkconfig.combined.in "")
foreach(SDKCONFIG_DEFAULT ${SDKCONFIG_DEFAULTS})
    file(READ ${SDKCONFIG_DEFAULT} CONTENTS)
    file(APPEND ${CMAKE_BINARY_DIR}/sdkconfig.combined.in "${CONTENTS}")
endforeach()
configure_file(${CMAKE_BINARY_DIR}/sdkconfig.combined.in ${CMAKE_BINARY_DIR}/sdkconfig.combined COPYONLY)
set(SDKCONFIG_DEFAULTS ${CMAKE_BINARY_DIR}/sdkconfig.combined)

# Include main IDF cmake file.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Generate individual dependencies.lock files based on chip target
idf_build_set_property(DEPENDENCIES_LOCK lockfiles/dependencies.lock.${IDF_TARGET})

# Define the project.
project(micropython)

# Check for lockfile changes and either warn or error depending on build type
message("Checking lockfile contents...")
execute_process(COMMAND git diff --exit-code lockfiles/ WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    RESULT_VARIABLE RES)
if (RES)
    # Maintainer builds (CI or autobuild runs) should fail if this has happened
    if($ENV{MICROPY_MAINTAINER_BUILD})
        message(FATAL_ERROR "Failing build as lockfiles are dirty (see above). Check that ESP-IDF versions match.")
    else()
        message(WARNING "Component lockfile contents have changed (see above). This may be due to building with a different ESP-IDF version. Please mention this output if reporting an issue with MicroPython.")
    endif()
endif()
