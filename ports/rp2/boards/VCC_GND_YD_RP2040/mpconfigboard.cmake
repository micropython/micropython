# CMake file for VCC-GND Studio  RP2040 boards

# VCC-GND Studio boards don't have official pico-sdk support so we define it
# See also: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})

# Freeze board.py
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

# Select the 8MB variant as the default
set(PICO_BOARD "ydrp2040_flash_8mb")

# Provide different variants for the downloads page
if(MICROPY_BOARD_VARIANT STREQUAL "flash_4mb")
    set(PICO_BOARD "ydrp2040_flash_4mb")
endif()

if(MICROPY_BOARD_VARIANT STREQUAL "flash_8mb")
    set(PICO_BOARD "ydrp2040_flash_8mb")
endif()

if(MICROPY_BOARD_VARIANT STREQUAL "flash_16mb")
    set(PICO_BOARD "ydrp2040_flash_16mb")
endif()
