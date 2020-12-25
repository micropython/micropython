set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.ble
    boards/sdkconfig.240mhz
    boards/sdkconfig.fast
    boards/sdkconfig.spiram
    )

set(MICROPY_FROZEN_MANIFEST ${BOARD_DIR}/manifest.py)
