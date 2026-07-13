include(boards/mpconfigboard_esp32s3_common.cmake)

# T-2CAN ships an ESP32-S3-WROOM-1U N16R8 module: 16MB flash + 8MB octal PSRAM.
list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.240mhz
    boards/sdkconfig.spiram_oct
    boards/sdkconfig.flash_qio_80m
    ${MICROPY_BOARD_DIR}/sdkconfig.board
)
