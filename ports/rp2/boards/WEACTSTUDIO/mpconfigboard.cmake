# CMake file for WeAct Studio RP2040 boards

# The WeAct Studio boards don't have official pico-sdk support so we define it
# See also: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})

# Freeze board.py
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 15728640)  # PICO_FLASH_SIZE_BYTES - (1 * 1024 * 1024) = 16MB - 1MB
endif()
