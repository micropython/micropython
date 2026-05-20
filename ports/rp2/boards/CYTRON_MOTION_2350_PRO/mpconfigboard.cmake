# cmake file for Raspberry Pi Pico2
set(PICO_BOARD "pico2")

# To change the gpio count for QFN-80
# set(PICO_NUM_GPIOS 48)

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 1048576)  # 1024 * 1024
endif()
