# CMake file for WeAct Studio RP2350B Core

# The Core is powered by an RP2350B with 48 GPIOs
set(PICO_NUM_GPIOS 48)

set(PICO_BOARD "weact_studio_rp2350b_core")

# Freeze manifest and modules
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 14680064)  # PICO_FLASH_SIZE_BYTES - (2 * 1024 * 1024) = 16MB - 2MB
endif()
