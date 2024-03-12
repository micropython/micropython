set(IDF_TARGET esp32s2)
set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    ${SDKCONFIG_IDF_VERSION_SPECIFIC}
    boards/sdkconfig.spiram_sx
    boards/sdkconfig.usb
    boards/UM_FEATHERS2NEO/sdkconfig.board
)

set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)
