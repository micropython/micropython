# cmake file for Raspberry Pi Pico
set(PICO_BOARD "pico")
set(PICO_PLATFORM "rp2040")

if(PICO_CYW43_SUPPORTED)
    include(enable_cyw43.cmake)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
    set(MICROPY_BOARD_PINS "${MICROPY_BOARD_DIR}/pins_cyw43.csv")
    # The cyw43 firmware and networking/bluetooth stacks enlarge the firmware
    # image, so reduce the filesystem to avoid overwriting it.
    if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
        set(MICROPY_HW_FLASH_STORAGE_BYTES 868352)  # 848 * 1024
    endif()
endif()

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 1441792)  # 1408 * 1024
endif()
