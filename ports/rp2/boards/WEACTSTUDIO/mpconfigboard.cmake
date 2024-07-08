# CMake file for WeAct Studio RP2040 boards

# The WeAct Studio boards don't have official pico-sdk support so we define it
# See also: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})

# Freeze board.py
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
