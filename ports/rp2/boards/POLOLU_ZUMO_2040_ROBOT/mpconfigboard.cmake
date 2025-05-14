# cmake file for Pololu Zumo 2040 Robot

# This board is not yet in upstream pico-sdk, so define it here
# See also: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})
