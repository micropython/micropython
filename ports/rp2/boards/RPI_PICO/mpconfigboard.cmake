# cmake file for Raspberry Pi Pico
set(PICO_BOARD "pico")
set(PICO_PLATFORM "rp2040")

if (PICO_CYW43_SUPPORTED)
    include(enable_cyw43.cmake)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
    set(PICO_PINS_CSV_NAME pins_cyw43.csv)
endif()
