list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.p4_pre_rev3
    boards/sdkconfig.p4_wifi_common
    boards/sdkconfig.p4_wifi_c6
)

list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="Generic ESP32P4 module with pre revision 3 chip and ESP32C6 hosted WIFI"
    MICROPY_PY_NETWORK_WLAN=1
    MICROPY_PY_BLUETOOTH=1
)
