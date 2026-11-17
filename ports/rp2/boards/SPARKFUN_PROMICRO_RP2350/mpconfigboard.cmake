# cmake file for SparkFun Pro Micro RP2350

set(PICO_BOARD "sparkfun_promicro_rp2350")
set(PICO_PLATFORM "rp2350")

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES 14680064)  # 14 * 1024 * 1024
endif()

# 8MB PSRAM. The chip-select (GPIO19) and size come from the pico-sdk board
# header (sparkfun_promicro_rp2350.h).
set(MICROPY_HW_ENABLE_PSRAM 1)
