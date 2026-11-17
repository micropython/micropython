list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.spiram_esp32
)

list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Generic ESP32 module with SPIRAM"
)
