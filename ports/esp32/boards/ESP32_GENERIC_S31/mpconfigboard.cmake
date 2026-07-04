set(IDF_TARGET esp32s31)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/sdkconfig.csi
    boards/sdkconfig.s31
)

set(MICROPY_PY_TINYUSB ON)
