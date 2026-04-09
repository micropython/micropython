# MCH2022 MicroPython board configuration
# Hardware: ESP32-WROVER-E-N16R8 (16MB flash, 8MB PSRAM)

set(IDF_TARGET esp32)

set(BOARD_DIR ${CMAKE_CURRENT_LIST_DIR})
set(PORT_DIR ${BOARD_DIR}/../..)   # ports/esp32
set(TOOLS_DIR ${PORT_DIR}/tools)

# Add driver_mch22 module to QSTR sources
list(APPEND MICROPY_SOURCE_QSTR
    ${CMAKE_CURRENT_SOURCE_DIR}/components/driver_mch22/modmch22.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/driver_sndmixer/modsndmixer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/driver_framebuffer/moddisplay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/pynvs/modnvs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/driver_rtcmem/modrtcmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/consts/modconsts.c
)

# Use our board-specific sdkconfig fragment
set(SDKCONFIG_DEFAULTS
    ${CMAKE_CURRENT_LIST_DIR}/sdkconfig.mch2022
    ${PORT_DIR}/.config
)

set(MICROPY_PY_MACHINE_I2S 0)

# Optional: include a frozen manifest if you want built-in Python modules
set(MICROPY_FROZEN_MANIFEST ${BOARD_DIR}/mch2022.py)

# Enable PSRAM (WROVER-E-N16R8 includes 8MB)
set(MICROPY_BOARD_ENABLE_PSRAM 1)

# Flash size (16MB)
set(MICROPY_BOARD_FLASH_SIZE 16MB)

# MCH2022 partition table
set(MICROPY_BOARD_PARTITION_TABLE_FILENAME "partitions-mch2022.csv")

set(MICROPY_PY_BLUETOOTH 1)
set(MICROPY_BLUETOOTH_NIMBLE 1)

set(MICROPY_SOURCE_BOARD
    ${MICROPY_BOARD_DIR}/board_init.c
)

set(MICROPY_FATFS_MAX_SS 4096)
