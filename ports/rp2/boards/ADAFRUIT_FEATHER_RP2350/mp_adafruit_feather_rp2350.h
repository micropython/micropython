#include "boards/adafruit_feather_rp2350.h"

// this specific STAGE2 is required for MicroPython to produce a working build
#define PICO_BOOT_STAGE2_CHOOSE_W25Q080 1

// Redefine cmake-level settings so they can be picked up by the build system.
pico_board_cmake_set_default(PICO_FLASH_SIZE_BYTES, (8 * 1024 * 1024))
pico_board_cmake_set_default(PICO_RP2350_A2_SUPPORTED, 1)
