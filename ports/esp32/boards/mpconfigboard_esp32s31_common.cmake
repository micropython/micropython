set(IDF_TARGET esp32s31)

list(APPEND SDKCONFIG_DEFAULTS
    boards/sdkconfig.s31_base
    boards/sdkconfig.ble
    boards/sdkconfig.spiram_oct
)
