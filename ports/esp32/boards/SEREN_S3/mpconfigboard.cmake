set(IDF_TARGET esp32s3)
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.usb
    boards/sdkconfig.ble
    boards/sdkconfig.spiram_sx
    boards/SEREN_S3/sdkconfig.board
)

# Override WLAN implementation
set(MICROPY_SOURCE_BOARD
    ${MICROPY_BOARD_DIR}/network_wlan_clamped.c
)
