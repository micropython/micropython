# cmake file for Seeed XIAO RP2350
set(PICO_BOARD "seeed_xiao_rp2350")

# To change the gpio count for QFN-80
# set(PICO_NUM_GPIOS 48)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 3145728)  # PICO_FLASH_SIZE_BYTES - 1024 * 1024 = 4MB - 1MB
endif()
