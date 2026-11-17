# cmake file for Machdyne Werkzeug
set(PICO_BOARD "machdyne_werkzeug")
set(PICO_PLATFORM "rp2040")

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 393216)  # 384 * 1024
endif()
