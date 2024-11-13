# cmake file for SparkFun Thing Plus RP2350

set(PICO_BOARD "sparkfun_thingplus_rp2350")
set(PICO_PLATFORM "rp2350")

include(enable_cyw43.cmake)

# Board specific version of the frozen manifest
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)