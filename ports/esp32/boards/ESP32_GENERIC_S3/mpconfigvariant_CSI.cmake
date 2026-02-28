set(SDKCONFIG_DEFAULTS
    ${SDKCONFIG_DEFAULTS}
    boards/sdkconfig.240mhz
    boards/sdkconfig.spiram_oct
    boards/sdkconfig.csi
    boards/ESP32_GENERIC_S3/sdkconfig.csi.opt
)

list(APPEND MICROPY_DEF_BOARD
    MICROPY_HW_BOARD_NAME="ESP32-S3 CSI Optimized"
    MICROPY_PY_NETWORK_WLAN_CSI=1
    MICROPY_PY_NETWORK_WLAN_CSI_DEFAULT_BUFFER_SIZE=8
)

