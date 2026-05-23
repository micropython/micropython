# cmake file for Waveshare RP2040-Zero
set(PICO_BOARD "waveshare_rp2040_zero")

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 1441792)  # 1408 * 1024
endif()
