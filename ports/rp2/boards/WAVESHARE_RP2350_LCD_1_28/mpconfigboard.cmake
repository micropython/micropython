# cmake file for Waveshare RP2350-LCD-1.28
set(PICO_BOARD "waveshare_rp2350_lcd_1.28")

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 3145728)  # 4MB flash - 1MB firmware = 3MB filesystem
endif()
