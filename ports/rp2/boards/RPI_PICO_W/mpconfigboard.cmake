# cmake file for Raspberry Pi Pico W

set(PICO_BOARD "pico_w")

include(enable_cyw43.cmake)

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
