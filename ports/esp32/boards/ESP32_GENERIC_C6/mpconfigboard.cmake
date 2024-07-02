set(IDF_TARGET esp32c6)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    ${SDKCONFIG_IDF_VERSION_SPECIFIC}
    boards/sdkconfig.ble
    boards/ESP32_GENERIC_C6/sdkconfig.board
)
