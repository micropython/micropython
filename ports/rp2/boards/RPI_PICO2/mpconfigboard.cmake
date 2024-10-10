# cmake file for Raspberry Pi Pico2
set(PICO_BOARD "pico2")

# To change the gpio count for QFN-80
# set(PICO_NUM_GPIOS 48)

if(PICO_CYW43_SUPPORTED)
    include(enable_cyw43.cmake)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
    set(MICROPY_BOARD_PINS "${MICROPY_BOARD_DIR}/pins_cyw43.csv")
endif()

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 3145728)  # 4MB flash - 1MB firmware = 3MB filesystem
endif()
