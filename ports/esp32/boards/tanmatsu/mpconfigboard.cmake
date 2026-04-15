# Tanmatsu MicroPython board configuration
# Hardware: ESP32P4 (16MB flash, 8MB PSRAM)

set(IDF_TARGET esp32p4)

set(BOARD_DIR ${CMAKE_CURRENT_LIST_DIR})
set(PORT_DIR ${BOARD_DIR}/../..)   # ports/esp32
set(TOOLS_DIR ${PORT_DIR}/tools)

# Add driver_mch22 module to QSTR sources
list(APPEND MICROPY_SOURCE_QSTR
    ${CMAKE_CURRENT_SOURCE_DIR}/components/pynvs/modnvs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/driver_rtcmem/modrtcmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/consts/modconsts.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/driver_framebuffer/moddisplay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/driver_badge_bsp/modbadgebsp.c
)

# Use our board-specific sdkconfig fragment
set(SDKCONFIG_DEFAULTS
    ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.tanmatsu
    boards/sdkconfig.p4	
    ${PORT_DIR}/.config
)

set(MICROPY_HW_ESP_NEW_I2C_DRIVER 1)

set(MICROPY_PY_MACHINE_I2S 0)

# Optional: include a frozen manifest if you want built-in Python modules
set(MICROPY_FROZEN_MANIFEST ${BOARD_DIR}/tanmatsu.py)

# Enable PSRAM (WROVER-E-N16R8 includes 8MB)
set(MICROPY_BOARD_ENABLE_PSRAM 1)

# Flash size (16MB)
set(MICROPY_BOARD_FLASH_SIZE 16MB)

# MCH2022 partition table
set(MICROPY_BOARD_PARTITION_TABLE_FILENAME "partitions-tanmatsu.csv")

set(MICROPY_PY_BLUETOOTH 0)
set(MICROPY_BLUETOOTH_NIMBLE 0)

set(MICROPY_PY_MACHINE_I2C_TARGET 0)
set(MICROPY_PY_ESPNOW             0)

set(MICROPY_SOURCE_BOARD
    ${MICROPY_BOARD_DIR}/board_init.c
)

set(MICROPY_FATFS_MAX_SS 4096)

# 1. Scan components for Kconfig files
execute_process(
    COMMAND python3 ${TOOLS_DIR}/kconfig_scan.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
