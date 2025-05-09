# CMake file for WeAct Studio RP2350B Core

# The Core is powered by an RP2350B with 48 GPIOs
set(PICO_NUM_GPIOS 48)

# The WeAct Studio boards don't have official pico-sdk support.
# So, add this board directory to the header search path and define PICO_BOARD
# which will instruct pico-sdk to look for weactstudio_rp2350b_core.h
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})
set(PICO_BOARD "weactstudio_rp2350b_core")

# Freeze manifest and modules
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
