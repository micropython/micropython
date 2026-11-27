# CMake file for Waveshare RP2350-POE-ETH-8DI-8RO.

# This board uses the 48-GPIO RP2350B.
set(PICO_PLATFORM "rp2350")
set(PICO_NUM_GPIOS 48)

# The board is not currently included in pico-sdk, so provide its header here.
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})
set(PICO_BOARD "waveshare_rp2350_poe_eth_8di_8ro")

set(PICO_FLASH_SIZE_BYTES 16777216)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 14680064)  # 14 * 1024 * 1024
endif()

# Enable the onboard W5500 and use it with the lwIP stack.
set(MICROPY_PY_NETWORK_WIZNET5K W5500)
set(MICROPY_PY_LWIP 1)
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
