list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.p4_pre_rev3
)

list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Generic ESP32P4 with pre revision 3 chip"
)
