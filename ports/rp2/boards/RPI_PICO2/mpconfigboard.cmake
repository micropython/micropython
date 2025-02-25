# cmake file for Raspberry Pi Pico2
set(PICO_BOARD "pico2")

# To change the gpio count for QFN-80
# set(PICO_NUM_GPIOS 48)

if (PICO_CYW43_SUPPORTED)
    include(enable_cyw43.cmake)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
    set(PICO_PINS_CSV_NAME pins_cyw43.csv)
endif()
