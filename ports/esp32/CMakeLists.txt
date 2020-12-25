cmake_minimum_required(VERSION 3.5)
set(SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig)

# Set a default board, if needed and load board.cmake
if(NOT MICROPY_BOARD)
    set(MICROPY_BOARD GENERIC)
endif()
include(boards/${MICROPY_BOARD}/board.cmake)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(micropython)
