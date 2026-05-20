# cmake file for nullbits Bit-C PRO

# The Bit-C PRO is not yet in upstream pico-sdk, so define it here
# See also: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})

set(PICO_BOARD "nullbits_bit_c_pro")

# Freeze board.py
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 3670016)  # PICO_FLASH_SIZE_BYTES - (1 * 512 * 1024) = 4MB - 512K // 512K reserved
endif()
