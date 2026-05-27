# CMake file for Waveshare RP2350B Core

# The Core is powered by an RP2350B with 48 GPIOs
set(PICO_NUM_GPIOS 48)

# The Waveshare boards don't have official pico-sdk support.
# So, add this board directory to the header search path and define PICO_BOARD
# which will instruct pico-sdk to look for waveshare_rp2350b_core.h
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})
set(PICO_BOARD "waveshare_rp2350b_core")

set(PICO_FLASH_SIZE_BYTES 16777216)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 14680064)  # 14 * 1024 * 1024
endif()
