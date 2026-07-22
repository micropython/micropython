# cmake file for Seeed Studio XIAO RP2040 Plus

# The XIAO RP2040 Plus is not yet in upstream pico-sdk, so define it here
# See also: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})

set(PICO_BOARD "seeed_xiao_rp2040_plus")
list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_FLASH_STORAGE_BYTES="(1408 * 1024)"
)
