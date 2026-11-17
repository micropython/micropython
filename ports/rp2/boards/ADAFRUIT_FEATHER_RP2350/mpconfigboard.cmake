# cmake file for Adafruit Feather RP2350

# override upstream board header with local copy
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_BOARD_DIR})

set(PICO_BOARD "mp_adafruit_feather_rp2350")

if(NOT DEFINED MICROPY_HW_FLASH_STORAGE_BYTES)
    set(MICROPY_HW_FLASH_STORAGE_BYTES  7340032) # 7 * 1024 * 1024
endif()

# To change the gpio count for QFN-80
# set(PICO_NUM_GPIOS 48)

# 8MB PSRAM on GPIO8.
set(MICROPY_HW_ENABLE_PSRAM 1)
set(MICROPY_HW_PSRAM_CS_PIN 8)
