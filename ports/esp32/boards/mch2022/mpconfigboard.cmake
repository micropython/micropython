# MCH2022 MicroPython board configuration
# Hardware: ESP32-WROVER-E-N16R8 (16MB flash, 8MB PSRAM)

set(IDF_TARGET esp32)

# Use our board-specific sdkconfig fragment
set(SDKCONFIG_DEFAULTS
    ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.mch2022
)

# Optional: include a frozen manifest if you want built-in Python modules
# set(MICROPY_FROZEN_MANIFEST ${CMAKE_CURRENT_LIST_DIR}/manifest.py)

# Enable PSRAM (WROVER-E-N16R8 includes 8MB)
set(MICROPY_BOARD_ENABLE_PSRAM 1)

# Flash size (16MB)
set(MICROPY_BOARD_FLASH_SIZE 16MB)

# MCH2022 partition table
set(MICROPY_BOARD_PARTITION_TABLE_FILENAME "partitions-mch2022.csv")

set(MICROPY_PY_BLUETOOTH 0)
set(MICROPY_BLUETOOTH_NIMBLE 0)
